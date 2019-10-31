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
      const foundation::UUID& uuid,
      HierarchyView* view) :
      uuid_(uuid),
      view_(view)
    {
      
    }

    //--------------------------------------------------------------------------
    HierarchyView* EntityCommand::view() const
    {
      return view_;
    }

    //--------------------------------------------------------------------------
    const foundation::UUID& EntityCommand::uuid() const
    {
      return uuid_;
    }

    //--------------------------------------------------------------------------
    HierarchyViewItem* EntityCommand::GetFromUUID(
      const foundation::UUID& uuid) const
    {
      if (uuid.IsNull() == true)
      {
        return nullptr;
      }

      return view_->FindItemByUUID(uuid);
    }

    //--------------------------------------------------------------------------
    HierarchyViewItem* EntityCommand::GetSelf() const
    {
      return GetFromUUID(uuid_);
    }

    //--------------------------------------------------------------------------
    CreateEntityCommand::CreateEntityCommand(
      const foundation::UUID& uuid,
      HierarchyView* view) :
      EntityCommand(uuid, view)
    {

    }

    //--------------------------------------------------------------------------
    void CreateEntityCommand::redo()
    {
      HierarchyView* hierarchy = view();
      HierarchyViewItem* item = hierarchy->CreateNewEntity();

      item->entity()->set_uuid(uuid());
    }

    //--------------------------------------------------------------------------
    void CreateEntityCommand::undo()
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
      const foundation::UUID& uuid,
      HierarchyView* view,
      int deleted_index) :
      EntityCommand(uuid, view),
      deleted_index_(deleted_index)
    {
      HierarchyViewItem* self = GetSelf();
      engine::TransformComponent* parent = 
        self->entity()->transform()->parent();

      deleted_from_ = 
        parent == nullptr ? foundation::UUID() : parent->entity()->uuid();
    }

    //--------------------------------------------------------------------------
    void DeleteEntityCommand::redo()
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
    void DeleteEntityCommand::undo()
    {
      HierarchyView* hierarchy = view();
      {
        HierarchyView::SceneChangedBlocker blocker(hierarchy);

        engine::Entity* ent = 
          hierarchy->CreateNewEntity(deleted_index_)->entity();

        if (serialization_data_.isEmpty() == true)
        {
          return;
        }

        foundation::LoadArchive archive;
        archive.FromJson(serialization_data_.toLatin1().data());

        archive(&ent);

        if (deleted_from_.IsNull() == false)
        {
          HierarchyViewItem* parent = GetFromUUID(deleted_from_);
          ent->transform()->SetParent(parent->entity()->transform());
        }
      }

      hierarchy->RefreshForCurrentScene();
    }

    //--------------------------------------------------------------------------
    ReparentEntityCommand::ReparentEntityCommand(
      const foundation::UUID& uuid,
      HierarchyView* view,
      const foundation::UUID& from,
      const foundation::UUID& to,
      int index_from,
      int index_to) :
      EntityCommand(uuid, view),
      from_(from),
      to_(to),
      index_from_(index_from),
      index_to_(index_to)
    {

    }

    //--------------------------------------------------------------------------
    void ReparentEntityCommand::redo()
    {
      HierarchyView::SceneChangedBlocker blocker(view());

      HierarchyViewItem* self = GetSelf();
      engine::TransformComponent* new_parent = nullptr;

      HierarchyViewItem* from = GetFromUUID(from_);
      HierarchyViewItem* to = GetFromUUID(to_);

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

      view()->UpdateSortIndices();
    }

    //--------------------------------------------------------------------------
    void ReparentEntityCommand::undo()
    {
      HierarchyView::SceneChangedBlocker blocker(view());

      HierarchyViewItem* self = GetSelf();
      engine::TransformComponent* new_parent = nullptr;

      HierarchyViewItem* from = GetFromUUID(from_);
      HierarchyViewItem* to = GetFromUUID(to_);

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

      view()->UpdateSortIndices();
    }
  }
}