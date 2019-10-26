#include "tools/editor/scene-editor/hierarchy_view.h"
#include "tools/editor/scene-editor/hierarchy_view_item.h"

#include "tools/editor/application/editor_application.h"

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
    HierarchyView::HierarchyView(EditorApplication* app, QWidget* parent) :
      QTreeWidget(parent),
      app_(app)
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
        OnSceneChanged(scene);
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

      OnSceneChanged(ss->current_scene());
    }

    //--------------------------------------------------------------------------
    void HierarchyView::OnSceneChanged(engine::Scene* scene)
    {
      QSignalBlocker blocker(this);

      foundation::HashSet<engine::Entity*> current_entities;

      scene->ForEachEntity([&](engine::Entity* ent)
      {
        TryAddEntity(ent);
        current_entities.insert(ent);

        return true;
      });

      ValidateCurrentEntities(current_entities);
    }

    //--------------------------------------------------------------------------
    void HierarchyView::TryAddEntity(engine::Entity* ent)
    {
      if (ent->transform() == nullptr)
      {
        return;
      }

      EntityMap::iterator it = entity_to_item_.find(ent);

      if (it != entity_to_item_.end())
      {
        HierarchyViewItem* old_item = it->second;
        ReparentItem(old_item, ent, true);
        old_item->Update();

        return;
      }

      HierarchyViewItem* new_item = new HierarchyViewItem(ent, this);
      ReparentItem(new_item, ent, false);

      entity_to_item_.insert(eastl::make_pair(ent, new_item));
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

        delete it->second;
        entity_to_item_.erase(it);
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
        &HierarchyView::CreateEntity);

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
        item->entity()->Destroy();
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

      engine::Entity* ent = item_a->entity();
      if (index.isValid() == false)
      {
        ent->transform()->SetParent(nullptr);
      }
      else
      {
        QTreeWidgetItem* dropped_on = itemFromIndex(index);
        HierarchyViewItem* item_b = static_cast<HierarchyViewItem*>(dropped_on);

        ent->transform()->SetParent(item_b->entity()->transform());
      }
    }

    //--------------------------------------------------------------------------
    void HierarchyView::CustomContextMenu(const QPoint& pos)
    {
      HierarchyViewItem* hovered_item = 
        static_cast<HierarchyViewItem*>(itemAt(pos));

      if (hovered_item == nullptr)
      {
        ShowRegularContextMenu(pos);
      }
      else
      {
        ShowEntityContextMenu(hovered_item, pos);
      }
    }

    //--------------------------------------------------------------------------
    void HierarchyView::CreateEntity()
    {
      foundation::Memory::Construct<engine::Entity>(
        &foundation::Memory::default_allocator(), GetCurrentScene());
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
  }
}