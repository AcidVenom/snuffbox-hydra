#pragma once

#include "engine/ecs/component.h"

#include <scripting/script_callback.h>

namespace snuffbox
{
  namespace engine
  {
    /**
    * @brief A script component that automatically calls callbacks within the
    *        entity, using callbacks setup from a global table/object
    *
    * @author Daniel Konings
    */
    SCRIPT_CLASS() class ScriptComponent :
      public ComponentBase<ScriptComponent, Components::kScript>
    {

    public:

      SCRIPT_NAME(ScriptComponent);

      /**
      * @see IComponent::IComponent
      */
      ScriptComponent(Entity* entity);

      /**
      * @brief Sets the behavior table/object from the global table and assigns
      *        all corresponding callbacks
      *
      * @param[in] behavior The name of the behavior to set
      */
      SCRIPT_FUNC() void SetBehavior(const foundation::String& behavior);

    protected:

      /**
      * @see IComponent::Update
      */
      void Update(float dt) override;

    private:

      scripting::ScriptCallback<> start_cb_; //!< The start callback
      scripting::ScriptCallback<float> update_cb_; //!< The update callback
    };

    //--------------------------------------------------------------------------
    CREATE_COMPONENT(ScriptComponent, Components::kScript);
  }
}
