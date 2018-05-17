#pragma once

#include "engine/ecs/component.h"
#include <scripting/script_class.h>

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
    SCRIPT_CLASS() class Entity : public scripting::ScriptClass
    {

    public:

      SCRIPT_NAME(Entity);

      /**
      * @brief Construct and add a TransformComponent
      */
      Entity();

      SCRIPT_CONSTRUCTOR(Entity);

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
      SCRIPT_FUNC(custom) IComponent* AddComponent(Components id);
      
      /**
      * @brief Removes the first found typed component
      *
      * @tparam T The component type
      */
      template <typename T>
      void RemoveComponent();

      /**
      * @brief Removes the first found typed component by ID
      *
      * @param[in] id The ID of the component to remove
      */
      SCRIPT_FUNC() void RemoveComponent(Components id);

      /**
      * @brief Removes all components of a certain type
      *
      * @tparam T The component type
      */
      template <typename T>
      void RemoveComponents();

      /**
      * @brief Removes all components of a certain type by ID
      *
      * @param[in] id The ID of the components to remove
      */
      SCRIPT_FUNC() void RemoveComponents(Components id);

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
      * @brief Checks if a typed component exists within this entity by ID
      *
      * @param[in] id The ID of the component to check
      *
      * @return Do we have more than 0 components of this type?
      */
      SCRIPT_FUNC() bool HasComponent(Components id);

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
      * @brief Retrieves the first typed component found within 
      *        this entity by ID
      *
      * @param[in] id The ID of the component to retrieve
      *
      * @return The found component, or nullptr if it doesn't exist
      */
      SCRIPT_FUNC(custom) IComponent* GetComponent(Components id);

      /**
      * @brief Retrieves all components of a certain component type
      *
      * @tparam T The component type
      *
      * @return The list of found components
      */
      template <typename T>
      foundation::Vector<T*> GetComponents();

      /**
      * @brief Retrieves all components of a certain component type by ID
      *
      * @param[in] id The ID of the component to retrieve
      *
      * @return The list of found components
      */
      SCRIPT_FUNC(custom) foundation::Vector<IComponent*> GetComponents(
        Components id);

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
      * @brief Checks if a component type is a valid type to use
      *
      * @param[in] id The ID to check
      *
      * @remarks This method asserts if the component type is not supported
      */
      static void IDCheck(Components id);

      /**
      * @brief Short-hand for the underlying structure of the components array
      */
      using ComponentArray = 
        foundation::Vector<foundation::UniquePtr<IComponent>>;

      /**
      * @brief Retrieves the component array of a certain component type
      *
      * @param[in] id The component type
      *
      * @return The underlying component array
      */
      ComponentArray& GetComponentArray(Components id);

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
      return static_cast<T*>(AddComponent(T::type_id));
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void Entity::RemoveComponent()
    {
      TypeCheck<T>();
      RemoveComponent(T::type_id);
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void Entity::RemoveComponents()
    {
      TypeCheck<T>();
      RemoveComponents(T::type_id);
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline bool Entity::HasComponent()
    {
      TypeCheck<T>();
      return HasComponent(T::type_id);
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline T* Entity::GetComponent()
    {
      TypeCheck<T>();
      return GetComponent(T::type_id);
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline foundation::Vector<T*> Entity::GetComponents()
    {
      TypeCheck<T>();

      foundation::Vector<T*> result;
      foundation::Vector<IComponent*> comps = GetComponents(T::type_id);

      result.resize(comps.size());

      for (size_t i = 0; i < comps.size(); ++i)
      {
        result.at(i) = static_cast<T*>(comps.at(i));
      }

      return result;
    }

    //--------------------------------------------------------------------------
    template <typename T>
    void Entity::TypeCheck()
    {
      const Components id = T::type_id;

      static_assert(eastl::is_base_of<ComponentBase<T, id>, T>::value == true,
        "Attempted to add an invalid component");
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