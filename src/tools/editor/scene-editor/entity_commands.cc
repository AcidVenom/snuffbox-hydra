#include "tools/editor/scene-editor/entity_commands.h"

#include "tools/editor/scene-editor/hierarchy_view.h"

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
  }
}