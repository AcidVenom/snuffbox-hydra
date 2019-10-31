#include "tools/editor/scene-editor/hierarchy_view.h"
#include "tools/editor/scene-editor/hierarchy_view_item.h"

#include "tools/editor/application/editor_application.h"

#include "tools/editor/scene-editor/entity_commands.h"

#include <engine/services/scene_service.h>
#include <engine/ecs/entity.h>
#include <engine/components/transform_component.h>

#include <QMenu>
#include <QDropEvent>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    HierarchyView::SceneChangedBlocker::SceneChangedBlocker(
      HierarchyView* view) :
      hierarchy(view)
    {
      hierarchy->blockSignals(true);
      hierarchy->block_scene_changed_ = true;
    }

    //--------------------------------------------------------------------------
    HierarchyView::SceneChangedBlocker::~SceneChangedBlocker()
    {
      hierarchy->block_scene_changed_ = false;
      hierarchy->blockSignals(false);
    }

    //--------------------------------------------------------------------------
    HierarchyView::HierarchyView(EditorApplication* app, QWidget* parent) :
      QTreeWidget(parent),
      app_(app),
      block_scene_changed_(false),
      hovered_item_(nullptr)
    {
      setSelectionMode(QAbstractItemView::SingleSelection);
      setColumnCount(1);
      setDragEnabled(true);
      viewport()->setAcceptDrops(true);
      setDropIndicatorShown(true);
      setDragDropMode(QAbstractItemView::InternalMove);

      setObjectName(QStringLiteral("HierarchyView"));
      setHeaderLabel(QStringLiteral("Entities"));
      
      engine::SceneService* ss = app_->GetService<engine::SceneService>();
      ss->set_on_scene_changed([&](engine::Scene* scene)
      {
        OnSceneDataChanged(scene);
      });

      setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);

      connect(
        this,
        &QTreeWidget::customContextMenuRequested,
        this,
        &HierarchyView::CustomContextMenu);

      connect(
        this,
        &QTreeWidget::itemChanged,
        this,
        &HierarchyView::OnItemChanged);

      RefreshForCurrentScene();
    }

    //--------------------------------------------------------------------------
    void HierarchyView::OnSceneDataChanged(engine::Scene* scene)
    {
      if (block_scene_changed_ == true)
      {
        return;
      }

      SceneChangedBlocker blocker(this);

      foundation::Vector<HierarchyViewItem*> was_expanded;
      was_expanded.resize(entity_to_item_.size(), nullptr);

      int expanded_count = 0;
      for (
        EntityMap::iterator it = entity_to_item_.begin();
        it != entity_to_item_.end();
        ++it)
      {
        HierarchyViewItem* item = it->second;

        if (item->isExpanded() == true)
        {
          was_expanded.at(expanded_count++) = item;
        }
      }

      foundation::HashSet<engine::Entity*> current_entities;

      foundation::Vector<engine::TransformComponent*> top_level = 
        scene->TopLevelTransforms();

      auto Sorter = [](
        const engine::TransformComponent* a, 
        const engine::TransformComponent* b)
      {
        return a->entity()->sort_index() < b->entity()->sort_index();
      };

      auto AddEntity = [&](engine::Entity* ent)
      {
        TryAddEntity(ent);
        current_entities.insert(ent);
      };

      foundation::Function<void(engine::TransformComponent*)> Recurse;

      Recurse = [&](engine::TransformComponent* top)
      {
        AddEntity(top->entity());

        foundation::Vector<engine::TransformComponent*> children = 
          top->children();

        std::sort(children.begin(), children.end(), Sorter);

        for (int i = 0; i < children.size(); ++i)
        {
          engine::TransformComponent* child = children.at(i);

          AddEntity(child->entity());
          Recurse(child);
        }
      };

      std::sort(top_level.begin(), top_level.end(), Sorter);
      for (int i = 0; i < top_level.size(); ++i)
      {
        Recurse(top_level.at(i));
      }

      ValidateCurrentEntities(current_entities);
      UpdateSortIndices();

      for (int i = 0; i < expanded_count; ++i)
      {
        HierarchyViewItem* expanded = was_expanded.at(i);
        for (
          EntityMap::iterator it = entity_to_item_.begin(); 
          it != entity_to_item_.end(); 
          ++it)
        {
          if (it->second == expanded)
          {
            expanded->setExpanded(true);
          }
        }
      }
    }

    //--------------------------------------------------------------------------
    HierarchyViewItem* HierarchyView::TryAddEntity(engine::Entity* ent)
    {
      if (ent->transform() == nullptr)
      {
        return nullptr;
      }

      EntityMap::iterator it = entity_to_item_.find(ent);

      if (it != entity_to_item_.end())
      {
        HierarchyViewItem* old_item = it->second;

        ReparentItem(old_item, ent, true);
        old_item->Update();

        return old_item;
      }

      HierarchyViewItem* new_item = new HierarchyViewItem(
        ent, 
        this);

      ent->set_sort_index(topLevelItemCount());
      ReparentItem(new_item, ent, false);

      entity_to_item_.insert(eastl::make_pair(ent, new_item));

      return new_item;
    }

    //--------------------------------------------------------------------------
    void HierarchyView::ReparentItem(
      HierarchyViewItem* item,
      engine::Entity* ent,
      bool was_added)
    {
      engine::TransformComponent* transform_parent = ent->transform()->parent();
      QTreeWidgetItem* old_parent = item->parent();
      HierarchyViewItem* current_parent = nullptr;

      if (transform_parent != nullptr)
      {
        engine::Entity* entity_parent = transform_parent->entity();
        EntityMap::iterator it = entity_to_item_.find(entity_parent);

        if (it == entity_to_item_.end())
        {
          TryAddEntity(entity_parent);
          current_parent = entity_to_item_.at(entity_parent);
        }
        else
        {
          current_parent = it->second;
        }
      }

      bool was_changed = item->SetParent(current_parent);

      if (was_changed == false)
      {
        return;
      }

      if (old_parent != nullptr)
      {
        old_parent->removeChild(item);
      }
      else
      {
        item = static_cast<HierarchyViewItem*>(
          takeTopLevelItem(indexOfTopLevelItem(item)));
      }

      if (current_parent == nullptr)
      {
        addTopLevelItem(item);
        return;
      }

      current_parent->addChild(item);
    }

    //--------------------------------------------------------------------------
    void HierarchyView::ValidateCurrentEntities(
      foundation::HashSet<engine::Entity*> current_entities)
    {
      foundation::Vector<engine::Entity*> to_remove;
      to_remove.resize(entity_to_item_.size(), nullptr);

      int count = 0;

      for (
        EntityMap::iterator it = entity_to_item_.begin(); 
        it != entity_to_item_.end(); 
        ++it)
      {
        if (current_entities.find(it->first) == current_entities.end())
        {
          to_remove.at(count++) = it->first;
        }
      }

      for (int i = 0; i < count; ++i)
      {
        engine::Entity* removed_entity = to_remove.at(i);
        EntityMap::iterator it = entity_to_item_.find(removed_entity);

        if (it != entity_to_item_.end())
        {
          delete it->second;
        }
      }
    }

    //--------------------------------------------------------------------------
    engine::Scene* HierarchyView::GetCurrentScene() const
    {
      return app_->GetService<engine::SceneService>()->current_scene();
    }

    //--------------------------------------------------------------------------
    void HierarchyView::ShowRegularContextMenu(const QPoint& pos)
    {
      QMenu menu("Actions");

      QAction create_ent("Create entity");
      connect(
        &create_ent,
        &QAction::triggered,
        this,
        &HierarchyView::OnCreateEntity);

      menu.addAction(&create_ent);

      menu.exec(mapToGlobal(pos));
    }

    //--------------------------------------------------------------------------
    void HierarchyView::ShowEntityContextMenu(
      HierarchyViewItem* item, 
      const QPoint& pos)
    {
      QMenu menu("Actions");

      QAction del("Delete");
      connect(&del, &QAction::triggered, this, [&]()
      {
        OnDeleteEntity();
      });

      QAction rename("Rename");
      connect(&rename, &QAction::triggered, this, [&]()
      {
        editItem(item);
      });

      menu.addAction(&del);
      menu.addAction(&rename);

      menu.exec(mapToGlobal(pos));
    }

    //--------------------------------------------------------------------------
    HierarchyViewItem* HierarchyView::FindItemByUUID(
      const foundation::UUID& uuid) const
    {
      for (
        EntityMap::const_iterator it = entity_to_item_.begin();
        it != entity_to_item_.end();
        ++it)
      {
        if (it->second->entity()->uuid() == uuid)
        {
          return it->second;
        }
      }

      return nullptr;
    }

    //--------------------------------------------------------------------------
    engine::Entity* HierarchyView::FindEntityByUUID(
      const foundation::UUID& uuid) const
    {
      HierarchyViewItem* item = FindItemByUUID(uuid);

      if (item == nullptr)
      {
        return nullptr;
      }

      return item->entity();
    }

    //--------------------------------------------------------------------------
    HierarchyViewItem* HierarchyView::FindItemByEntity(
      engine::Entity* ent) const
    {
      EntityMap::const_iterator it = entity_to_item_.find(ent);

      if (it == entity_to_item_.end())
      {
        return nullptr;
      }

      return it->second;
    }

    //--------------------------------------------------------------------------
    void HierarchyView::RefreshForCurrentScene()
    {
      OnSceneDataChanged(
        app_->GetService<engine::SceneService>()->current_scene());
    }

    //--------------------------------------------------------------------------
    void HierarchyView::UpdateSortIndices()
    {
      for (
        EntityMap::iterator it = entity_to_item_.begin();
        it != entity_to_item_.end();
        ++it)
      {
        engine::Entity* ent = it->first;
        HierarchyViewItem* item = it->second;

        QTreeWidgetItem* parent = item->parent();

        if (parent == nullptr)
        {
          ent->set_sort_index(indexOfTopLevelItem(item));
        }
        else
        {
          ent->set_sort_index(parent->indexOfChild(item));
        }
      }
    }

    //--------------------------------------------------------------------------
    void HierarchyView::Clear()
    {
      clear();
      entity_to_item_.clear();
    }

    //--------------------------------------------------------------------------
    void HierarchyView::dropEvent(QDropEvent* evt)
    {
      QModelIndex index = indexAt(evt->pos());

      QList<QTreeWidgetItem*> selected_items = selectedItems();
      QTreeWidgetItem* selected_item = 
        selected_items.size() > 0 ? selected_items.first() : nullptr;

      HierarchyViewItem* item_a = 
        static_cast<HierarchyViewItem*>(selected_item);

      if (item_a == nullptr)
      {
        return;
      }

      int from_index = -1;
      int to_index = -1;

      QTreeWidgetItem* dropped_on = itemFromIndex(index);
      HierarchyViewItem* item_b = 
        static_cast<HierarchyViewItem*>(dropped_on);

      QAbstractItemView::DropIndicatorPosition pos = 
        dropIndicatorPosition();

      bool was_above = pos == QAbstractItemView::AboveItem;
      bool was_below = pos == QAbstractItemView::BelowItem;
      bool was_dropped_on = was_above == false && was_below == false;

      QTreeWidgetItem* parent = item_a->parent();
      from_index = parent == nullptr ? 
        indexOfTopLevelItem(item_a) : parent->indexOfChild(item_a);

      if (item_b != nullptr)
      {
        parent = item_b->parent();
        if (was_dropped_on == true)
        {
          to_index = item_b->childCount();
        }
        else
        {
          to_index = parent == nullptr ?
            indexOfTopLevelItem(item_b) : parent->indexOfChild(item_b);

          item_b = static_cast<HierarchyViewItem*>(parent);
        }
      }
      else
      {
        to_index = topLevelItemCount() - 1;
      }

      HierarchyViewItem* parent_item = 
        static_cast<HierarchyViewItem*>(item_a->parent());

      foundation::UUID from_uuid = 
        parent_item == nullptr ? foundation::UUID() :
        parent_item->entity()->uuid();
      foundation::UUID to_uuid =
        item_b == nullptr ? foundation::UUID() : 
        item_b->entity()->uuid();
        
      ReparentEntityCommand* cmd = new ReparentEntityCommand(
        item_a->entity()->uuid(),
        this,
        from_uuid,
        to_uuid,
        from_index,
        to_index);

      undo_stack_.push(cmd);
    }

    //--------------------------------------------------------------------------
    HierarchyViewItem* HierarchyView::CreateNewEntity(int index)
    {
      engine::Entity* ent = foundation::Memory::Construct<engine::Entity>(
        &foundation::Memory::default_allocator(), GetCurrentScene());

      HierarchyViewItem* item = TryAddEntity(ent);

      if (index != -1)
      {
        QTreeWidgetItem* to_move = nullptr;
        QTreeWidgetItem* parent = item->parent();

        if (parent == nullptr)
        {
          to_move = takeTopLevelItem(indexOfTopLevelItem(item));
          insertTopLevelItem(index, to_move);
        }
        else
        {
          to_move = parent->takeChild(parent->indexOfChild(item));
          parent->insertChild(index, to_move);
        }

        item->entity()->set_sort_index(index);
      }

      return item;
    }

    //--------------------------------------------------------------------------
    void HierarchyView::UnmapItem(const HierarchyViewItem* item)
    {
      EntityMap::iterator it = entity_to_item_.find(item->entity());

      if (it != entity_to_item_.end())
      {
        entity_to_item_.erase(it);
      }
    }

    //--------------------------------------------------------------------------
    void HierarchyView::CustomContextMenu(const QPoint& pos)
    {
      hovered_item_ = static_cast<HierarchyViewItem*>(itemAt(pos));

      if (hovered_item_ == nullptr)
      {
        ShowRegularContextMenu(pos);
      }
      else
      {
        ShowEntityContextMenu(hovered_item_, pos);
      }
    }

    //--------------------------------------------------------------------------
    void HierarchyView::OnItemChanged(QTreeWidgetItem* item, int column)
    {
      HierarchyViewItem* edited_item = static_cast<HierarchyViewItem*>(item);

      QString old_name = edited_item->entity()->name().c_str();
      QString new_name = edited_item->text(0);

      if (old_name != new_name)
      {
        edited_item->entity()->set_name(new_name.toLatin1().data());
      }
    }

    //--------------------------------------------------------------------------
    void HierarchyView::OnCreateEntity()
    {
      CreateEntityCommand* command =
        new CreateEntityCommand(foundation::UUID::Create(), this);

      undo_stack_.push(command);
    }

    //--------------------------------------------------------------------------
    void HierarchyView::OnDeleteEntity()
    {
      if (hovered_item_ == nullptr)
      {
        return;
      }

      int delete_index = -1;

      QTreeWidgetItem* parent = hovered_item_->parent();
      if (parent != nullptr)
      {
        delete_index = parent->indexOfChild(hovered_item_);
      }
      else
      {
        delete_index = indexOfTopLevelItem(hovered_item_);
      }

      DeleteEntityCommand* command = new DeleteEntityCommand(
        hovered_item_->entity()->uuid(), 
        this, 
        delete_index);

      undo_stack_.push(command);
    }

    //--------------------------------------------------------------------------
    void HierarchyView::OnSceneChanged(const QString& scene_name)
    {
      Clear();
      RefreshForCurrentScene();
    }

    //--------------------------------------------------------------------------
    void HierarchyView::Undo()
    {
      if (undo_stack_.canUndo() == false)
      {
        return;
      }

      undo_stack_.undo();
    }

    //--------------------------------------------------------------------------
    void HierarchyView::Redo()
    {
      if (undo_stack_.canRedo() == false)
      {
        return;
      }

      undo_stack_.redo();
    }
  }
}