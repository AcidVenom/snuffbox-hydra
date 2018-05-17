#pragma once

#include "engine/definitions/components.h"

#define CREATE_COMPONENT(type, id)                                             \
template <>                                                                    \
inline snuffbox::engine::IComponent*                                           \
snuffbox::engine::IComponent::CreateComponent<id>()                            \
{                                                                              \
  return snuffbox::foundation::Memory::Construct<type>(                        \
    &snuffbox::foundation::Memory::default_allocator());                       \
}                                                                              

namespace snuffbox
{
  namespace engine
  {
    class Entity;

    /**
    * @brief The interface of every component, which can be attached to
    *        entities to update them
    *
    * @author Daniel Konings
    */
    class IComponent
    {

      friend Entity;

    protected:

      /**
      * @brief Used to define behavior of the component when it is created
      */
      virtual void Create();

      /**
      * @brief Used to define behavior when the component is updated from
      *        within the entity
      */
      virtual void Update();

      /**
      * @brief Used to define behavior for when the component is destroyed
      */
      virtual void Destroy();

      /**
      * @brief This function is to be specialized by a Components ID to create
      *        the actual typed component
      */
      template <Components C>
      static IComponent* CreateComponent();

    public:

      /**
      * @brief Used to create components from script by script name, from
      *        the base class
      *
      * @return The type name as in the script state
      */
      virtual const char* GetScriptName() const = 0;

      /**
      * @brief Virtual destructor
      */
      virtual ~IComponent();
    };

    /**
    * @brief The base class for every component to use which will automatically
    *        assign a type ID to the corresponding component
    *
    * @tparam C The component type to use for the type ID
    *
    * @author Daniel Konings
    */
    template <typename T, Components C>
    class ComponentBase : public IComponent
    {

    public:

      /**
      * @return The ID of this component
      */
      static const Components type_id = C;

      /**
      * @see IComponent::GetScriptName
      */
      const char* GetScriptName() const override;

      /**
      * @brief Virtual destructor
      */
      virtual ~ComponentBase();
    };

    //--------------------------------------------------------------------------
    template <Components C>
    inline IComponent* IComponent::CreateComponent()
    {
      foundation::Logger::LogVerbosity<1>(
        foundation::LogChannel::kEngine,
        foundation::LogSeverity::kWarning,
        "Attempted to create a component with an undefined create function");

      return nullptr;
    }

    //--------------------------------------------------------------------------
    template <typename T, Components C>
    inline const char* ComponentBase<T, C>::GetScriptName() const
    {
#ifndef SNUFF_NSCRIPTING
      return T::ScriptName();
#else
      return "undefined";
#endif
    }

    //--------------------------------------------------------------------------
    template <typename T, Components C>
    inline ComponentBase<T, C>::~ComponentBase()
    {

    }
  }
}