#include "tools/editor/scene-editor/entity_commands.h"

#include "tools/editor/scene-editor/hierarchy_view.h"
#include "tools/editor/scene-editor/hierarchy_view_item.h"

#include <foundation/serialization/save_archive.h>
#include <foundation/serialization/load_archive.h>
#include <engine/ecs/entity.h>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    EntityCommand::EntityCommand(const QUuid& uuid, HierarchyView* view) :
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
    const QUuid& EntityCommand::uuid() const
    {
      return uuid_;
    }

    //--------------------------------------------------------------------------
    CreateEntityCommand::CreateEntityCommand(
      const QUuid& uuid, 
      HierarchyView* view) :
      EntityCommand(uuid, view)
    {

    }

    //--------------------------------------------------------------------------
    void CreateEntityCommand::redo()
    {
      HierarchyView* hierarchy = view();
      hierarchy->CreateNewEntity(uuid());
    }

    //--------------------------------------------------------------------------
    void CreateEntityCommand::undo()
    {
      HierarchyView* hierarchy = view();
      engine::Entity* ent = hierarchy->FindEntityByUUID(uuid());

      if (ent == nullptr)
      {
        return;
      }

      ent->Destroy();
    }

    //--------------------------------------------------------------------------
    DeleteEntityCommand::DeleteEntityCommand(
      const QUuid& uuid, 
      HierarchyView* view,
      int deleted_from) :
      EntityCommand(uuid, view),
      deleted_from_(deleted_from)
    {

    }

    //--------------------------------------------------------------------------
    void DeleteEntityCommand::redo()
    {
      HierarchyView* hierarchy = view();
      engine::Entity* ent = hierarchy->FindEntityByUUID(uuid());

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
      engine::Entity* ent = hierarchy->CreateNewEntity(uuid(), deleted_from_);

      if (serialization_data_.isEmpty() == true)
      {
        return;
      }

      foundation::LoadArchive archive;
      archive.FromJson(serialization_data_.toLatin1().data());

      archive(&ent);

      hierarchy->RefreshForCurrentScene();
    }

    //--------------------------------------------------------------------------
    ReparentEntityCommand::ReparentEntityCommand(
      const QUuid& uuid,
      HierarchyView* view,
      HierarchyViewItem* from,
      HierarchyViewItem* to) :
      EntityCommand(uuid, view),
      from_(from),
      to_(to),
      index_from_(-1),
      index_to_(-1)
    {
      index_from_ = GetIndexForItem(from_);
      index_to_ = GetIndexForItem(to_);
    }

    //--------------------------------------------------------------------------
    void ReparentEntityCommand::redo()
    {
      QTreeWidgetItem* self = nullptr;
      if (from_ == nullptr)
      {
        self = view()->takeTopLevelItem(index_from_);
      }
      else
      {
        self = from_->takeChild(index_from_);
      }

      if (to_ == nullptr)
      {
        view()->insertTopLevelItem(index_to_, self);
      }
      else
      {
        to_->insertChild(index_to_, self);
      }
    }

    //--------------------------------------------------------------------------
    void ReparentEntityCommand::undo()
    {

    }

    //--------------------------------------------------------------------------
    int ReparentEntityCommand::GetIndexForItem(HierarchyViewItem* item) const
    {
      HierarchyView* hierarchy = view();
      HierarchyViewItem* self = hierarchy->FindItemByUUID(uuid());

      if (item == nullptr)
      {
        return hierarchy->indexOfTopLevelItem(self);
      }

      return item->indexOfChild(self);
    }
  }
}