#pragma once

#include "engine/ecs/component.h"

namespace snuffbox
{
  namespace engine
  {
    class TransformComponent : public ComponentBase<Components::kTransform>
    {

    };

    //--------------------------------------------------------------------------
    template <>
    IComponent* IComponent::CreateComponent<Components::kTransform>()
    {
      return foundation::Memory::Construct<TransformComponent>(
        &foundation::Memory::default_allocator());
    }
  }
}