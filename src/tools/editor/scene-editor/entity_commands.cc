#include "tools/editor/scene-editor/entity_commands.h"

#include "tools/editor/scene-editor/hierarchy_view.h"
#include "tools/editor/scene-editor/hierarchy_view_item.h"

#include <foundation/serialization/save_archive.h>
#include <foundation/serialization/load_archive.h>
#include <foundation/containers/function.h>

#include <engine/ecs/entity.h>
#include <engine/components/transform_component.h>

#include <qdebug.h>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    EntityCommand::EntityCommand(
      const QString& index,
      HierarchyView* view) :
      index_(index),
      view_(view)
    {
      qDebug() << index_;
    }

    //--------------------------------------------------------------------------
    HierarchyView* EntityCommand::view() const
    {
      return view_;
    }

    //--------------------------------------------------------------------------
    const QString& EntityCommand::index() const
    {
      return index_;
    }

    //--------------------------------------------------------------------------
    void EntityCommand::redo()
    {
      HierarchyView::SceneChangedBlocker scope(view_);
      RedoImpl();
    }

    //--------------------------------------------------------------------------
    void EntityCommand::undo()
    {
      HierarchyView::SceneChangedBlocker scope(view_);
      UndoImpl();
    }

    //--------------------------------------------------------------------------
    HierarchyViewItem* EntityCommand::GetFromIndex(
      const QString& index) const
    {
      if (index.isEmpty() == true)
      {
        return nullptr;
      }

      return view_->FindItemByIndex(index);
    }

    //--------------------------------------------------------------------------
    HierarchyViewItem* EntityCommand::GetSelf() const
    {
      return GetFromIndex(index_);
    }

    //--------------------------------------------------------------------------
    CreateEntityCommand::CreateEntityCommand(
      const QString& index,
      HierarchyView* view) :
      EntityCommand(index, view)
    {

    }

    //--------------------------------------------------------------------------
    void CreateEntityCommand::RedoImpl()
    {
      HierarchyView* hierarchy = view();
      hierarchy->CreateNewEntity();
    }

    //--------------------------------------------------------------------------
    void CreateEntityCommand::UndoImpl()
    {
      HierarchyView* hierarchy = view();
      engine::Entity* ent = GetSelf()->entity();

      if (ent == nullptr)
      {
        return;
      }

      ent->Destroy();
    }

    //--------------------------------------------------------------------------
    DeleteEntityCommand::DeleteEntityCommand(
      const QString& index,
      HierarchyView* view,
      int deleted_from) :
      EntityCommand(index, view),
      deleted_from_(deleted_from)
    {

    }

    //--------------------------------------------------------------------------
    void DeleteEntityCommand::RedoImpl()
    {
      HierarchyView* hierarchy = view();
      engine::Entity* ent = GetSelf()->entity();

      if (ent == nullptr)
      {
        return;
      }

      foundation::SaveArchive archive;
      archive(ent);

      serialization_data_ = archive.ToMemory().c_str();
      ent->Destroy();
    }

    //--------------------------------------------------------------------------
    void DeleteEntityCommand::UndoImpl()
    {
      HierarchyView* hierarchy = view();
      engine::Entity* ent = 
        hierarchy->CreateNewEntity(deleted_from_)->entity();

      if (serialization_data_.isEmpty() == true)
      {
        return;
      }

      foundation::LoadArchive archive;
      archive.FromJson(serialization_data_.toLatin1().data());

      archive(&ent);
    }

    //--------------------------------------------------------------------------
    ReparentEntityCommand::ReparentEntityCommand(
      const QString& index,
      HierarchyView* view,
      const QString& from,
      const QString& to,
      int index_from,
      int index_to) :
      EntityCommand(index, view),
      from_model_idx_(from),
      to_model_idx_(to),
      index_from_(index_from),
      index_to_(index_to)
    {

    }

    //--------------------------------------------------------------------------
    void ReparentEntityCommand::RedoImpl()
    {
      HierarchyViewItem* self = GetSelf();
      engine::TransformComponent* new_parent = nullptr;

      HierarchyViewItem* from = GetFromIndex(from_model_idx_);
      HierarchyViewItem* to = GetFromIndex(to_model_idx_);

      if (to != nullptr)
      {
        new_parent = to->entity()->transform();
      }

      self->entity()->transform()->SetParent(new_parent);

      if (from == nullptr)
      {
        self = static_cast<HierarchyViewItem*>(
          view()->takeTopLevelItem(index_from_));
      }
      else
      {
        self = static_cast<HierarchyViewItem*>(from->takeChild(index_from_));
      }

      if (to == nullptr)
      {
        view()->insertTopLevelItem(index_to_, self);
      }
      else
      {
        to->insertChild(index_to_, self);
      }
    }

    //--------------------------------------------------------------------------
    void ReparentEntityCommand::UndoImpl()
    {
      HierarchyViewItem* self = GetSelf();
      engine::TransformComponent* new_parent = nullptr;

      HierarchyViewItem* from = GetFromIndex(from_model_idx_);
      HierarchyViewItem* to = GetFromIndex(to_model_idx_);

      if (from != nullptr)
      {
        new_parent = from->entity()->transform();
      }

      self->entity()->transform()->SetParent(new_parent);

      if (to == nullptr)
      {
        self = static_cast<HierarchyViewItem*>(
          view()->takeTopLevelItem(index_to_));
      }
      else
      {
        self = static_cast<HierarchyViewItem*>(to->takeChild(index_to_));
      }

      if (from == nullptr)
      {
        view()->insertTopLevelItem(index_from_, self);
      }
      else
      {
        from->insertChild(index_from_, self);
      }
    }
  }
}