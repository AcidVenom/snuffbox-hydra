#include "engine/ecs/component.h"
#include "engine/ecs/entity.h"
#include "engine/components/transform_component.h"

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
    void IComponent::Start()
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
    TransformComponent* IComponent::transform() const
    {
      return entity_->GetComponent<TransformComponent>();
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