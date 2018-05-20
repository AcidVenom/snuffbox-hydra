#include "engine/ecs/component.h"

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    IComponent::IComponent(Entity* entity) :
      entity_(entity),
      active_(true)
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
    void IComponent::set_active(bool active)
    {
      active_ = active;
    }

    //--------------------------------------------------------------------------
    bool IComponent::active() const
    {
      return active_;
    }

    //--------------------------------------------------------------------------
    IComponent::~IComponent()
    {

    }
  }
}