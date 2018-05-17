#include "engine/ecs/entity.h"

#include "engine/components/transform_component.h"

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    Entity::Entity()
    {
      AddComponent<TransformComponent>();
    }

    //--------------------------------------------------------------------------
    IComponent* Entity::AddComponent(Components id)
    {
      IComponent* ptr = CreateComponentByID(id);
      ptr->Create();

      components_[static_cast<size_t>(id)].push_back(
        foundation::Memory::MakeUnique(ptr));

      return ptr;
    }

    //--------------------------------------------------------------------------
    const Entity::ComponentCreateArray& Entity::ComponentCreators()
    {
      static ComponentCreateArray funcs;

      if (funcs.size() == 0)
      {
        funcs.resize(static_cast<size_t>(Components::kCount));
        AssignComponentCreator<Components::kTransform>(funcs);
      }

      return funcs;
    }

    //--------------------------------------------------------------------------
    IComponent* Entity::CreateComponentByID(Components id)
    {
      const ComponentCreateArray& c = ComponentCreators();
      return c.at(static_cast<size_t>(id))();
    }
  }
}