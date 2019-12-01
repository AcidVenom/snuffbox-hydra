#include "tools/editor/scene-editor/hierarchy_view.h"
#include "tools/editor/scene-editor/hierarchy_view_item.h"

#include "tools/editor/application/editor_application.h"

#include "tools/editor/scene-editor/entity_commands.h"

#include <engine/services/scene_service.h>
#include <engine/ecs/entity.h>
#include <engine/components/transform_component.h>

#include <QMenu>
#include <QDropEvent>
#include <QMetaType>

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

      connect(
        this,
        &QTreeWidget::itemPressed,
        this,
        &HierarchyView::OnSelectionChanged);

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

        UpdateParentIndex(old_item);
        old_item->Update();

        return old_item;
      }

      HierarchyViewItem* new_item = nullptr;
      engine::TransformComponent* parent = ent->transform()->parent();

      if (parent == nullptr)
      {
        new_item = new HierarchyViewItem(ent, this);
        ent->set_sort_index(topLevelItemCount() - 1);
      }
      else
      {
        new_item = new HierarchyViewItem(
          ent, 
          this, 
          FindItemByEntity(parent->entity()));
      }
      
      UpdateParentIndex(new_item);

      entity_to_item_.insert(eastl::make_pair(ent, new_item));

      return new_item;
    }

    //--------------------------------------------------------------------------
    void HierarchyView::UpdateParentIndex(HierarchyViewItem* item)
    {
      engine::Entity* ent = item->entity();
      engine::TransformComponent* parent = ent->transform()->parent();

      if (parent == nullptr)
      {
        int idx = indexOfTopLevelItem(item);
        if (idx == -1)
        {
          QTreeWidgetItem* parent_item = item->parent();
          if (parent_item != nullptr)
          {
            item = static_cast<HierarchyViewItem*>(
              parent_item->takeChild(parent_item->indexOfChild(item)));
          }
          addTopLevelItem(item);
          idx = indexOfTopLevelItem(item);
        }

        if (ent->sort_index() != -1)
        {
          if (idx != ent->sort_index())
          {
            QTreeWidgetItem* taken_item = takeTopLevelItem(idx);
            insertTopLevelItem(ent->sort_index(), taken_item);
          }
        }
        else
        {
          ent->set_sort_index(idx);
        }
      }
      else
      {
        HierarchyViewItem* parent_item = FindItemByEntity(parent->entity());
        if (parent_item == nullptr)
        {
          return;
        }

        int idx = parent_item->indexOfChild(item);
        if (idx == -1)
        {
          if ((idx = indexOfTopLevelItem(item)) >= 0)
          {
            item = static_cast<HierarchyViewItem*>(takeTopLevelItem(idx));
          }
          else
          {
            QTreeWidgetItem* old_parent = item->parent();
            if (old_parent != nullptr && old_parent != parent_item)
            {
              item = static_cast<HierarchyViewItem*>(
                parent_item->takeChild(parent_item->indexOfChild(item)));
            }
          }

          parent_item->addChild(item);
          idx = parent_item->indexOfChild(item);
        }
        
        if (ent->sort_index() != -1)
        {
          if (idx != ent->sort_index())
          {
            QTreeWidgetItem* taken_item = parent_item->takeChild(idx);
            parent_item->insertChild(ent->sort_index(), taken_item);
          }
        }
        else
        {
          ent->set_sort_index(idx);
        }
      }
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

      bool was_blocked = signalsBlocked() == true;
      if (was_blocked == true)
      {
        blockSignals(false);
      }

      for (int i = 0; i < count; ++i)
      {
        engine::Entity* removed_entity = to_remove.at(i);
        EntityMap::iterator it = entity_to_item_.find(removed_entity);

        if (it != entity_to_item_.end())
        {
          OnEntityDeleted(removed_entity);
          delete it->second;
        }
      }

      if (was_blocked == true)
      {
        blockSignals(true);
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
      ClearSelection();
      clear();
      entity_to_item_.clear();
      undo_stack_.clear();
    }

    //--------------------------------------------------------------------------
    void HierarchyView::dropEvent(QDropEvent* evt)
    {
      evt->setDropAction(Qt::DropAction::IgnoreAction);
      evt->accept();

      QTreeWidget::dropEvent(evt);

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

          if (item_b->indexOfChild(item_a) >= 0)
          {
            if (item_b->childCount() == 1)
            {
              return;
            }

            --to_index;
          }
        }
        else
        {
          to_index = parent == nullptr ?
            indexOfTopLevelItem(item_b) : parent->indexOfChild(item_b);

          if (was_below == true)
          {
            ++to_index;
          }

          item_b = static_cast<HierarchyViewItem*>(parent);
        }
      }
      else
      {
        to_index = topLevelItemCount();

        if (indexOfTopLevelItem(item_a) >= 0)
        {
          if (topLevelItemCount() == 1)
          {
            return;
          }

          --to_index;
        }
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

      PushUndoCommand(cmd);
    }

    //--------------------------------------------------------------------------
    void HierarchyView::mouseReleaseEvent(QMouseEvent* evt)
    {
      if (itemAt(evt->pos()) == nullptr)
      {
        ClearSelection();
      }

      QTreeWidget::mouseReleaseEvent(evt);
    }

    //--------------------------------------------------------------------------
    HierarchyViewItem* HierarchyView::CreateNewEntity()
    {
      engine::Entity* ent = foundation::Memory::Construct<engine::Entity>(
        &foundation::Memory::default_allocator(), GetCurrentScene());

      HierarchyViewItem* item = TryAddEntity(ent);

      return item;
    }

    //--------------------------------------------------------------------------
    void HierarchyView::UnmapItem(const HierarchyViewItem* item)
    {
      if (GetSelectedItem() == item)
      {
        ClearSelection();
      }

      EntityMap::iterator it = entity_to_item_.find(item->entity());

      if (it != entity_to_item_.end())
      {
        entity_to_item_.erase(it);
      }
    }

    //--------------------------------------------------------------------------
    HierarchyViewItem* HierarchyView::GetSelectedItem() const
    {
      QList<QTreeWidgetItem*> selected_items = selectedItems();
      HierarchyViewItem* selected_item = selected_items.size() == 0 ?
        nullptr : static_cast<HierarchyViewItem*>(selected_items.first());

      return selected_item;
    }

    //--------------------------------------------------------------------------
    void HierarchyView::OnEntityDeleted(engine::Entity* ent)
    {
      HierarchyViewItem* item = GetSelectedItem();
      if (item != nullptr && ent == item->entity())
      {
        ClearSelection();
      }
    }

    //--------------------------------------------------------------------------
    void HierarchyView::ClearSelection()
    {
      selectionModel()->clearSelection();
      emit ItemSelectionChanged(nullptr);
    }

    //--------------------------------------------------------------------------
    bool HierarchyView::CanUndoRedo() const
    {
      return EditorApplication::Instance()->state() == 
        EditorApplication::EditorStates::kEditing;
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
      engine::Entity* ent = edited_item->entity();

      QString old_name = ent->name().c_str();
      QString new_name = edited_item->text(0);

      if (old_name != new_name)
      {
        PropertyEntityCommand* cmd = 
          new PropertyEntityCommand(ent->uuid(), this, "name");

        cmd->Set(foundation::String(new_name.toLatin1().data()));

        PushUndoCommand(cmd);
      }
    }

    //--------------------------------------------------------------------------
    void HierarchyView::OnCreateEntity()
    {
      CreateEntityCommand* command =
        new CreateEntityCommand(foundation::UUID::Create(), this);

      PushUndoCommand(command);
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

      engine::Entity* to_delete = hovered_item_->entity();

      DeleteEntityCommand* command = new DeleteEntityCommand(
        to_delete->uuid(),
        this, 
        delete_index);

      PushUndoCommand(command);
    }

    //--------------------------------------------------------------------------
    void HierarchyView::OnSelectionChanged(QTreeWidgetItem* item, int column)
    {
      emit ItemSelectionChanged(static_cast<HierarchyViewItem*>(item));
    }

    //--------------------------------------------------------------------------
    void HierarchyView::OnSceneChanged(const QString& scene_name)
    {
      Clear();
      RefreshForCurrentScene();
    }

    //--------------------------------------------------------------------------
    void HierarchyView::OnSceneRefreshed()
    {
      Clear();
    }

    //--------------------------------------------------------------------------
    void HierarchyView::Undo()
    {
      if (CanUndoRedo() == false)
      {
        return;
      }

      if (undo_stack_.canUndo() == false)
      {
        return;
      }

      undo_stack_.undo();
    }

    //--------------------------------------------------------------------------
    void HierarchyView::Redo()
    {
      if (CanUndoRedo() == false)
      {
        return;
      }

      if (undo_stack_.canRedo() == false)
      {
        return;
      }

      undo_stack_.redo();
    }

    //--------------------------------------------------------------------------
    void HierarchyView::PushUndoCommand(QUndoCommand* cmd)
    {
      if (CanUndoRedo() == false)
      {
        cmd->redo();
        delete cmd;
        return;
      }

      undo_stack_.push(cmd);
    }
  }
}