#pragma once

#include "engine/definitions/components.h"

#include <scripting/script_class.h>

#include <foundation/serialization/serializable.h>
#include <foundation/memory/memory.h>

#define CREATE_COMPONENT(type, id)                                             \
template <>                                                                    \
inline snuffbox::engine::IComponent*                                           \
snuffbox::engine::IComponent::CreateComponent<id>(Entity* entity)              \
{                                                                              \
  return snuffbox::foundation::Memory::Construct<type>(                        \
    &snuffbox::foundation::Memory::default_allocator(),                        \
    entity);                                                                   \
}                                                                              

namespace snuffbox
{
  namespace engine
  {
    class Entity;
    class TransformComponent;

    /**
    * @brief The interface of every component, which can be attached to
    *        entities to update them
    *
    * @author Daniel Konings
    */
    class IComponent :
      public scripting::ScriptClass, 
      public foundation::ISerializable
    {

      friend Entity;

    protected:

      /**
      * @brief Create a component with an entity as its parent
      *
      * @param[in] entity The parent entity
      */
      IComponent(Entity* entity);

      /**
      * @brief Used to define behavior of the component when it is created
      */
      virtual void Create();

      /**
      * @brief Used to define behavior when Entity::Start is called
      */
      virtual void Start();

      /**
      * @brief Used to define behavior when the component is updated from
      *        within the entity
      *
      * @param[in] dt The current delta-time of the application
      */
      virtual void Update(float dt);

      /**
      * @brief Used to define behavior for when the component is destroyed
      */
      virtual void Destroy();

      /**
      * @brief This function is to be specialized by a Components ID to create
      *        the actual typed component
      *
      * @param[in] entity The parent entity that owns this component
      */
      template <Components C>
      static IComponent* CreateComponent(Entity* entity);

    public:

      /**
      * @brief Used to create components from script by script name, from
      *        the base class
      *
      * @return The type name as in the script state
      */
      virtual const char* GetScriptName() const = 0;

      /**
      * @return The entity parent of this component
      */
      Entity* entity() const;

      /**
      * @return The transform component of the parent entity
      */
      TransformComponent* transform() const;

      /**
      * @brief Sets if this component is currently active
      */
      void set_active(bool active);

      /**
      * @return Is this component currently active?
      */
      bool active() const;

      virtual void Serialize(foundation::SaveArchive& archive) const override = 0;
      virtual void Deserialize(foundation::LoadArchive& archive) override = 0;

      /**
      * @brief Virtual destructor
      */
      virtual ~IComponent();

    private:

      Entity* entity_; //!< The entity parent of this component
      bool active_; //!< Is this component currently active?
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

    protected:

      /**
      * @see IComponent::IComponent
      */
      ComponentBase(Entity* entity);

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
    inline IComponent* IComponent::CreateComponent(Entity* entity)
    {
      foundation::Logger::LogVerbosity<1>(
        foundation::LogChannel::kEngine,
        foundation::LogSeverity::kWarning,
        "Attempted to create a component with an undefined create function");

      return nullptr;
    }

    //--------------------------------------------------------------------------
    template <typename T, Components C>
    inline ComponentBase<T, C>::ComponentBase(Entity* entity) :
      IComponent(entity)
    {

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