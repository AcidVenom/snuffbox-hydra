#pragma once

#include "engine/ecs/component.h"

#include <foundation/containers/vector.h>
#include <foundation/memory/memory.h>
#include <foundation/auxiliary/logger.h>

#include <cstddef>

namespace snuffbox
{
  namespace engine
  {
    /**
    * @brief An entity class to hold a multitude of components that are updated
    *        through the Scene
    *
    * @author Daniel Konings
    */
    class Entity
    {

    public:

      /**
      * @brief Construct and add a TransformComponent
      */
      Entity();

      /**
      * @brief Adds a typed component and returns it
      *
      * @tparam T The component type
      *
      * @return The created component, or nullptr if it couldn't be added
      */
      template <typename T>
      T* AddComponent();

      /**
      * @brief Adds a component by ID
      *
      * @param[in] id The component ID
      *
      * @see Entity::CreateComponentByID
      */
      IComponent* AddComponent(Components id);
      
      /**
      * @brief Removes the first found typed component
      *
      * @tparam T The component type
      */
      template <typename T>
      void RemoveComponent();

      /**
      * @brief Removes all components of a certain type
      *
      * @tparam T The component type
      */
      template <typename T>
      void RemoveComponents();

      /**
      * @brief Checks if a typed component exists within this entity
      *
      * @tparam T The component type
      *
      * @return Do we have more than 0 components of this type?
      */
      template <typename T>
      bool HasComponent();

      /**
      * @brief Retrieves the first typed component found within this entity
      *
      * @tparam T The component type
      *
      * @return The found component, or nullptr if it doesn't exist
      */
      template <typename T>
      T* GetComponent();

      /**
      * @brief Retrieves all components of a certain component type
      *
      * @tparam T The component type
      *
      * @return The list of found components
      */
      template <typename T>
      foundation::Vector<T*> GetComponents();

    protected:

      /**
      * @brief Checks if a component type is actually derived from its
      *        respective component base
      *
      * This function will static_assert if the type is incorrect
      *
      * @tparam T The type to check
      */
      template <typename T>
      static void TypeCheck();

      /**
      * @brief Short-hand for the underlying structure of the components array
      */
      using ComponentArray = 
        foundation::Vector<foundation::UniquePtr<IComponent>>;

      /**
      * @brief Retrieves the component array of a certain component type
      *
      * @tparam T The component type
      *
      * @return The underlying component array
      */
      template <typename T>
      ComponentArray& GetComponentArray();

      /**
      * @brief A type definition for the functions to create components
      *
      * @see IComponent::CreateComponent
      */
      using ComponentCreateFunc = IComponent*(*)();

      /**
      * @brief A short-hand to easily reach the component creation functions
      */
      using ComponentCreateArray = foundation::Vector<ComponentCreateFunc>;

      /**
      * @brief Retrieves the list of component creation functions by ID
      *
      * The return array will be a list of function pointers that have a
      * 1 on 1 mapping to their respective component IDs
      *
      * @remarks The first time this function is called, all assignments to
      *          the creation array will be done
      *
      * @see Entity::AssignComponentCreator
      *
      * @return The function pointer array
      */
      static const ComponentCreateArray& ComponentCreators();

      /**
      * @brief Assigns a component creation function by component ID, to the
      *        list of component creation functions
      *
      * @remarks This function recurses until Components::kCount is found.
      *          This function is a compile-time unrolled loop.
      *
      * @tparam C The current ID
      *
      * @param[in] arr The list to add the component creation function to
      */
      template <Components C>
      static void AssignComponentCreator(ComponentCreateArray& arr);

      /**
      * @brief Used to create a component by ID, using the creation functions
      *        assigned in Entity::AssignComponentCreator
      *
      * @param[in] id The ID of the component to create
      *
      * @return The base component pointer to the created component
      */
      static IComponent* CreateComponentByID(Components id);

    private:

      /**
      * @brief The list of components, of which each component can be
      *        contained multiple times
      */
      ComponentArray components_[static_cast<size_t>(Components::kCount)];
    };

    //--------------------------------------------------------------------------
    template <typename T>
    inline T* Entity::AddComponent()
    {
      TypeCheck<T>();
      ComponentArray& c = GetComponentArray<T>();

      IComponent* comp = IComponent::CreateComponent<T::type_id>();
      comp->Create();

      T* ptr = static_cast<T*>(comp);

      c.push_back(foundation::Memory::MakeUnique<T>(ptr));

      return ptr;
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void Entity::RemoveComponent()
    {
      TypeCheck<T>();

      if (HasComponent<T>() == false)
      {
        return;
      }

      ComponentArray& c = GetComponentArray<T>();

      ComponentArray::iterator it = c.begin();
      it->get()->Destroy();

      c.erase(it);
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void Entity::RemoveComponents()
    {
      TypeCheck<T>();

      if (HasComponent<T>() == false)
      {
        return;
      }

      ComponentArray& c = GetComponentArray<T>();

      for (size_t i = 0; i < c.size(); ++i)
      {
        c.at(i)->Destroy();
      }

      c.clear();
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline bool Entity::HasComponent()
    {
      TypeCheck<T>();
      ComponentArray& c = GetComponentArray<T>();

      return c.size() > 0;
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline T* Entity::GetComponent()
    {
      TypeCheck<T>();

      if (HasComponent<T>() == false)
      {
        return nullptr;
      }

      ComponentArray& c = GetComponentArray<T>();

      return static_cast<T*>(c.at(0).get());
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline foundation::Vector<T*> Entity::GetComponents()
    {
      TypeCheck<T>();

      foundation::Vector<T*> result;
      ComponentArray& c = GetComponentArray<T>();

      result.resize(c.size());

      for (size_t i = 0; i < c.size(); ++i)
      {
        result.at(i) = static_cast<T*>(c.at(i).get());
      }

      return result;
    }

    //--------------------------------------------------------------------------
    template <typename T>
    void Entity::TypeCheck()
    {
      const Components id = T::type_id;

      static_assert(eastl::is_base_of<ComponentBase<id>, T>::value == true,
        "Attempted to add an invalid component");
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline Entity::ComponentArray& Entity::GetComponentArray()
    {
      TypeCheck<T>();

      Components id = T::type_id;
      foundation::Logger::Assert(id < Components::kCount,
        "Attempted to index a component type that is larger than the maximum\
         number of allowed components");

      return components_[static_cast<size_t>(id)];
    }

    //--------------------------------------------------------------------------
    template <Components C>
    inline void Entity::AssignComponentCreator(ComponentCreateArray& arr)
    {
      const size_t id = static_cast<size_t>(C);
      arr.at(id) = &IComponent::CreateComponent<C>;
      
      const size_t next = id + 1;
      AssignComponentCreator<static_cast<Components>(next)>(arr);
    }

    //--------------------------------------------------------------------------
    template <>
    inline void Entity::AssignComponentCreator<Components::kCount>(
      ComponentCreateArray& arr)
    {

    }
  }
}