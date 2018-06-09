#include "engine/components/script_component.h"
#include "engine/ecs/entity.h"

#include "engine/application/application.h"
#include "engine/services/script_service.h"

#include <foundation/serialization/save_archive.h>
#include <foundation/serialization/load_archive.h>

#include <sparsed/script_component.gen.cc>

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    ScriptComponent::ScriptComponent(Entity* entity) :
      ComponentBase<ScriptComponent, Components::kScript>(entity),
      behavior_("undefined")
    {

    }

    //--------------------------------------------------------------------------
    void ScriptComponent::SetBehavior(const foundation::String& behavior)
    {
      bool call_start = false;
      if (behavior_ != behavior)
      {
        call_start = true;
      }

      behavior_ = behavior;

      ScriptService* ss = Application::Instance()->GetService<ScriptService>();
      
      scripting::ScriptCallback<>::FromClass(
        ss->state(), behavior, "Start", &start_cb_);

      scripting::ScriptCallback<float>::FromClass(
        ss->state(), behavior, "Update", &update_cb_);

      if (call_start == false)
      {
        return;
      }

      Start();
    }

    //--------------------------------------------------------------------------
    const foundation::String& ScriptComponent::behavior() const
    {
      return behavior_;
    }

    //--------------------------------------------------------------------------
    void ScriptComponent::Start()
    {
      if (start_cb_.is_valid() == false)
      {
        Refresh();
      }

      start_cb_.CallContext(this);
    }

    //--------------------------------------------------------------------------
    void ScriptComponent::Update(float dt)
    {
      if (update_cb_.is_valid() == false)
      {
        Refresh();
      }

      update_cb_.CallContext(this, dt);
    }

    //--------------------------------------------------------------------------
    void ScriptComponent::Refresh()
    {
      SetBehavior(behavior_);
    }

    //--------------------------------------------------------------------------
    void ScriptComponent::Serialize(foundation::SaveArchive& archive) const
    {
      archive(SET_ARCHIVE_PROP(behavior_));
    }

    //--------------------------------------------------------------------------
    void ScriptComponent::Deserialize(foundation::LoadArchive& archive)
    {
      archive(GET_ARCHIVE_PROP(behavior_));
    }
  }
}