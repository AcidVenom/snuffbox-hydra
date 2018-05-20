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
      ComponentBase<ScriptComponent, Components::kScript>(entity)
    {

    }

    //--------------------------------------------------------------------------
    void ScriptComponent::SetBehavior(const foundation::String& behavior)
    {
      ScriptService* ss = Application::Instance()->GetService<ScriptService>();
      
      scripting::ScriptCallback<>::FromClass(
        ss->state(), behavior, "Start", &start_cb_);

      scripting::ScriptCallback<float>::FromClass(
        ss->state(), behavior, "Update", &update_cb_);
    }

    //--------------------------------------------------------------------------
    void ScriptComponent::Update(float dt)
    {
      update_cb_.CallContext(this, dt);
    }
  }
}