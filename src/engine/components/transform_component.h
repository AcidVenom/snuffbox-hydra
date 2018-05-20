#pragma once

#include "engine/ecs/component.h"

#include <scripting/script_class.h>

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
    SCRIPT_CLASS() class TransformComponent
      : 
      public ComponentBase<TransformComponent, Components::kTransform>
    {

    public:

      SCRIPT_NAME(TransformComponent);

      /**
      * @see IComponent::IComponent
      */
      TransformComponent(Entity* entity);

      /**
      * @brief Checks if this component has a specific transform component child
      *
      * @param[in] child The child to check for
      *
      * @return The index of the found child, or -1 if it doesn't exist
      */
      SCRIPT_FUNC() int HasChild(TransformComponent* child);

      /**
      * @brief Adds a transform component as a child to this component and
      *        assigns the parent of the child to this component
      *
      * @param[in] child The child to add
      */
      SCRIPT_FUNC() void Attach(TransformComponent* child);

      /**
      * @brief Removes a transform component child from this component and
      *        sets the parent of that child to null
      *
      * @param[in] child The child to detach
      */
      SCRIPT_FUNC() void Detach(TransformComponent* child);

      /**
      * @brief Removes all transform component children from this component
      */
      SCRIPT_FUNC() void DetachAll();

      /**
      * @see TransformComponent::Detach
      *
      * @brief Removes the child by index
      *
      * @param[in] idx The index of the child to remove
      */
      SCRIPT_FUNC() void DetachAt(int idx);

      /**
      * @brief Retrieves the child at a specified index
      *
      * @param[in] idx The index to retrieve the child from
      *
      * @return The found child, or nullptr if it does not exist
      */
      SCRIPT_FUNC() TransformComponent* GetChildAt(int idx) const;

      /**
      * @brief Sets the parent transform of this component
      */
      SCRIPT_FUNC() void SetParent(TransformComponent* parent);

      /**
      * @return The parent transform of this component
      */
      SCRIPT_FUNC() TransformComponent* parent() const;

      /**
      * @return The children transform components of this component
      */
      SCRIPT_FUNC(custom) 
        const foundation::Vector<TransformComponent*>& children() const;

    protected:

      /**
      * @brief Only sets the parent variable, but doesn't do any 
      *        detaching/attaching
      *
      * @param[in] parent The parent to set
      */
      void SetParentRaw(TransformComponent* parent);

      /**
      * @see IComponent::Update
      */
      void Update(float dt) override;

    private:

      TransformComponent* parent_; //!< The parent transform of this component

      /**
      * @brief The children transform components of this component
      */
      foundation::Vector<TransformComponent*> children_;
    };

    //--------------------------------------------------------------------------
    CREATE_COMPONENT(TransformComponent, Components::kTransform);
  }
}