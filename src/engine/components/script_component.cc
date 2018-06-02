#include "engine/components/script_component.h"
#include "engine/ecs/entity.h"

#include "engine/application/application.h"
#include "engine/services/script_service.h"

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
    Entity* ScriptComponent::GetEntity()
    {
      return entity();
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
  }
}