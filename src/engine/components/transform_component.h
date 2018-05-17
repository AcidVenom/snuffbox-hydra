#pragma once

#include "engine/ecs/component.h"

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
    class TransformComponent : public ComponentBase<Components::kTransform>
    {

    };

    //--------------------------------------------------------------------------
    CREATE_COMPONENT(TransformComponent, Components::kTransform);
  }
}