#include "tools/editor/windows/hierarchy_view.h"
#include "tools/editor/windows/main_window.h"
#include "tools/editor/application/editor_application.h"

#include <engine/ecs/entity.h>
#include <engine/ecs/scene.h>
#include <engine/services/scene_service.h>
#include <engine/components/transform_component.h>

#include <qmenu.h>
#include <qaction.h>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    HierarchyViewItem::HierarchyViewItem(
      engine::TransformComponent* transform) 
      :
      transform_(transform)
    {
      setText(0, transform_->entity()->name().c_str());
    }

    //--------------------------------------------------------------------------
    engine::TransformComponent* HierarchyViewItem::transform() const
    {
      return transform_;
    }

    //--------------------------------------------------------------------------
    HierarchyView::HierarchyView(
      QTreeWidget* widget, 
      engine::SceneService* scene_service)
      :
      tree_(widget),
      context_menu_(nullptr),
      add_entity_(nullptr),
      remove_entity_(nullptr),
      scene_service_(scene_service)
    {
      AddContextMenu();
    }

    //--------------------------------------------------------------------------
    void HierarchyView::AddContextMenu()
    {
      context_menu_ = new QMenu();
      tree_->setContextMenuPolicy(Qt::CustomContextMenu);

      add_entity_ = new QAction("Add Entity", context_menu_);
      context_menu_->addAction(add_entity_);

      remove_entity_ = new QAction("Remove Entity", context_menu_);
      context_menu_->addAction(remove_entity_);

      connect(
        tree_,
        &QTreeWidget::customContextMenuRequested,
        this,
        &HierarchyView::OnCustomContextMenu);
    }

    //--------------------------------------------------------------------------
    engine::Scene* HierarchyView::CurrentScene()
    {
      return scene_service_->current_scene();
    }

    //--------------------------------------------------------------------------
    void HierarchyView::AddEntity()
    {
      foundation::Memory::Construct<engine::Entity>(
        &foundation::Memory::default_allocator());

      OnHierarchyChanged();
    }

    //--------------------------------------------------------------------------
    void HierarchyView::RemoveEntity(HierarchyViewItem* item)
    {
      foundation::Memory::Destruct<engine::Entity>(item->transform()->entity());

      OnHierarchyChanged();
    }

    //--------------------------------------------------------------------------
    void HierarchyView::OnHierarchyChanged()
    {
      engine::Scene* scene = CurrentScene();

      tree_->clear();

      const foundation::Vector<engine::TransformComponent*>& hierarchy =
        scene->hierarchy();

      for (size_t i = 0; i < hierarchy.size(); ++i)
      {
        AddSceneChild(hierarchy.at(i));
      }
    }

    //--------------------------------------------------------------------------
    void HierarchyView::AddSceneChild(
      engine::TransformComponent* child,
      HierarchyViewItem* item)
    {
      const engine::Entity* ent = child->entity();
      const foundation::String& name = ent->name();

      HierarchyViewItem* new_item = new HierarchyViewItem(child);

      if (item == nullptr)
      {
        tree_->addTopLevelItem(new_item);
      }
      else
      {
        item->addChild(new_item);
      }

      const foundation::Vector<engine::TransformComponent*>& children =
        child->children();

      for (size_t i = 0; i < children.size(); ++i)
      {
        AddSceneChild(children.at(i), new_item);
      }
    }

    //--------------------------------------------------------------------------
    void HierarchyView::OnCustomContextMenu(const QPoint& p)
    {
      QModelIndex index = tree_->indexAt(p);
      remove_entity_->setEnabled(index.isValid());

      QAction* action = context_menu_->exec(tree_->viewport()->mapToGlobal(p));

      if (action == add_entity_)
      {
        AddEntity();
      }
      else if (action == remove_entity_)
      {
        RemoveEntity(static_cast<HierarchyViewItem*>(tree_->itemAt(p)));
      }
    }
  }
}