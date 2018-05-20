#include "engine/ecs/component.h"

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    IComponent::IComponent(Entity* entity) :
      entity_(entity)
    {

    }

    //--------------------------------------------------------------------------
    void IComponent::Create()
    {

    }

    //--------------------------------------------------------------------------
    void IComponent::Update(float dt)
    {

    }

    //--------------------------------------------------------------------------
    void IComponent::Destroy()
    {

    }

    //--------------------------------------------------------------------------
    Entity* IComponent::entity() const
    {
      return entity_;
    }

    //--------------------------------------------------------------------------
    IComponent::~IComponent()
    {

    }
  }
}