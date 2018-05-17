#pragma once

#include "engine/ecs/component.h"

#include <scripting/script_class.h>

namespace snuffbox
{
  namespace engine
  {
    /**
    * @brief A transform component to affine transformations on an entity with
    *
    * This is the only required component, all other components are optional.
    * Every entity gets a transform component assigned when they are created.
    * This component can also not be removed, or have duplicate entries within
    * the entity.
    *
    * @author Daniel Konings
    */
    SCRIPT_CLASS() class TransformComponent 

      : 
      public ComponentBase<TransformComponent, Components::kTransform>,
      public scripting::ScriptClass
    {

    public:

      SCRIPT_NAME(TransformComponent);

      SCRIPT_FUNC() void SetSomeValue(float x)
      {
        x_ = x;
      }

      SCRIPT_FUNC() float GetValue()
      {
        return x_;
      }

    private:

      float x_;
    };

    //--------------------------------------------------------------------------
    CREATE_COMPONENT(TransformComponent, Components::kTransform);
  }
}