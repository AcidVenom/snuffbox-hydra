#pragma once

#include "engine/ecs/component.h"

#include <foundation/containers/vector.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

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

    protected:

      /**
      * @brief The different type of "dirty" states, which are used to only
      *        update transforms if they are indeed dirty
      */
      enum DirtyFlags : uint8_t
      {
        kNone = 0, //!< No flags set
        kChild = 1 << 0, //!< A child is dirty
        kSelf = 1 << 1 //!< The component itself is dirty
      };

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
      SCRIPT_FUNC(custom) void SetParent(TransformComponent* parent);

      /**
      * @return The parent transform of this component
      */
      SCRIPT_FUNC() TransformComponent* parent() const;

      /**
      * @return The children transform components of this component
      */
      SCRIPT_FUNC(custom) 
        const foundation::Vector<TransformComponent*>& children() const;

      /**
      * @brief Sets the world position of this transform component
      *
      * @remarks This updates the underlying matrices, if dirty
      *
      * @param[in] position The position to set
      */
      SCRIPT_FUNC() void SetPosition(const glm::vec3& position);

      /**
      * @brief Sets the local position of this transform component
      *
      * @param[in] position The position to set
      */
      SCRIPT_FUNC() void SetLocalPosition(const glm::vec3& position);

      /**
      * @return The world position of this transform component
      *
      * @remarks This updates the underlying matrices, if dirty
      */
      SCRIPT_FUNC() glm::vec3 GetPosition();

      /**
      * @return The local position of this transform component
      */
      SCRIPT_FUNC() const glm::vec3& GetLocalPosition();

      /**
      * @brief Sets the rotation of this transform component, by quaternion
      *
      * @param[in] rotation The rotation quaternion
      */
      SCRIPT_FUNC() void SetRotation(const glm::quat& rotation);

      /**
      * @brief Sets the rotation of this transform component, from a world-space
      *        forward and up axis
      *
      * @param[in] forward The forward axis
      * @param[in] up The up axis
      *
      * @remarks This requires a uniform scaling matrix to function properly
      */
      SCRIPT_FUNC() void SetWorldRotation(
        const glm::vec3& forward, 
        const glm::vec3& up);

      /**
      * @brief Sets the rotation of this transform component, by euler angles
      *
      * @param[in] rotation The rotation euler angle vector
      *
      * @remarks The euler angles are in degrees
      */
      SCRIPT_FUNC() void SetRotationEuler(const glm::vec3& rotation);

      /**
      * @brief Sets the rotation of this transform component, around an axis
      *
      * @param[in] axis The axis to set the rotation angle around
      * @param[in] angle The angle to set the rotation at
      *
      * @remarks The angle is in degrees
      */
      SCRIPT_FUNC() void RotateAxis(const glm::vec3& axis, float angle);

      /**
      * @return The rotation quaternion of this transform component
      */
      SCRIPT_FUNC() const glm::quat& GetRotation() const;

      /**
      * @return The rotation of this transform component, in euler angles
      *
      * @remarks The euler angles are in degrees
      */
      SCRIPT_FUNC() glm::vec3 GetRotationEuler() const;

      /**
      * @brief Sets the scale of this transform component
      *
      * @param[in] scale The scale to set
      */
      SCRIPT_FUNC() void SetScale(const glm::vec3& scale);

      /**
      * @brief Sets the scale of this transform component in world-space
      *
      * @param[in] scale The scale to set
      *
      * @remarks For this to function properly, the scaling needs to be uniform
      */
      SCRIPT_FUNC() void SetWorldScale(const glm::vec3& scale);

      /**
      * @return The scale of this transform component
      */
      SCRIPT_FUNC() const glm::vec3& GetScale() const;

      /**
      * @return The scale of this transform component in world space
      */
      SCRIPT_FUNC() glm::vec3 GetWorldScale() const;

      /**
      * @brief Transforms a point using the local-to-world matrix
      *
      * @remarks This updates the underlying matrices, if dirty
      *
      * @param[in] point The point to transform
      *
      * @return The transformed point
      */
      SCRIPT_FUNC() glm::vec3 TransformPoint(const glm::vec3& point);

      /**
      * @brief Transforms a point using the world-to-local matrix
      *
      * @remarks This updates the underlying matrices, if dirty
      * @remarks The vector implicitly has a W value of 1.0
      *
      * @param[in] point The point to transform
      *
      * @return The transformed point
      */
      SCRIPT_FUNC() glm::vec3 InvTransformPoint(const glm::vec3& point);

      /**
      * @brief Transforms a direction using the local-to-world matrix
      *
      * @remarks This updates the underlying matrices, if dirty
      * @remarks The vector implicitly has a W value of 0.0
      *
      * @param[in] point The direction to transform
      *
      * @return The transformed direction
      */
      SCRIPT_FUNC() glm::vec3 TransformDirection(const glm::vec3& direction);

      /**
      * @brief Transforms a direction using the world-to-local matrix
      *
      * @remarks This updates the underlying matrices, if dirty
      * @remarks The vector implicitly has a W value of 0.0
      *
      * @param[in] point The direction to transform
      *
      * @return The transformed direction
      */
      SCRIPT_FUNC() glm::vec3 InvTransformDirection(const glm::vec3& direction);

      /**
      * @return The "up" vector, relative to this transform
      *
      * @remarks This updates the underlying matrices, if dirty
      */
      SCRIPT_FUNC() glm::vec3 Up();

      /**
      * @return The "forward" vector, relative to this transform
      *
      * @remarks This updates the underlying matrices, if dirty
      */
      SCRIPT_FUNC() glm::vec3 Forward();

      /**
      * @return The "right" vector, relative to this transform
      *
      * @remarks This updates the underlying matrices, if dirty
      */
      SCRIPT_FUNC() glm::vec3 Right();

      /**
      * @brief Translates over this transform's axes
      *
      * @remarks This updates the underlying matrices, if dirty
      *
      * @param[in] translation The translation to apply
      */
      SCRIPT_FUNC() void TranslateLocal(const glm::vec3& translation);

      /**
      * @brief Translates the transform in world space
      *
      * @remarks This updates the underlying matrices, if dirty
      *
      * @param[in] translation The translation to apply
      */
      SCRIPT_FUNC() void Translate(const glm::vec3& translation);

      /**
      * @return The local to world matrix
      */
      const glm::mat4x4& local_to_world() const;

      /**
      * @return The world to local matrix
      */
      const glm::mat4x4& world_to_local() const;

    protected:

      /**
      * @brief Only sets the parent variable, but doesn't do any 
      *        detaching/attaching
      *
      * @param[in] parent The parent to set
      */
      void SetParentRaw(TransformComponent* parent);

      /**
      * @brief Sets the dirty flags of this transform for updating accordingly
      *        when TransformComponent::UpdateMatrices is called
      *
      * @param[in] flag The type of "dirtiness" to set
      */
      void MarkDirty(DirtyFlags flag);

      /**
      * @brief Updates the matrices of the entire parent hierarchy this
      *        transform component belongs to
      */
      void UpdateFromTop();

      /**
      * @brief Updates the matrices of this component by traversing its parent
      *        tree and applying all properties
      *
      * @remarks This removes the "dirty" flag if it was set
      */
      void UpdateMatrices();

    public:

      /**
      * @see IComponent::Update
      */
      void Update(float dt) override;

      /**
      * @see ISerializable::Serialize
      */
      void Serialize(foundation::SaveArchive& archive) const override;

      /**
      * @see ISerializable::Deserialize
      */
      void Deserialize(foundation::LoadArchive& archive) override;

      /**
      * @brief Detaches this component from its parent if it has one
      */
      ~TransformComponent();

    private:

      TransformComponent* parent_; //!< The parent transform of this component

      /**
      * @brief The children transform components of this component
      */
      foundation::Vector<TransformComponent*> children_;

      glm::vec3 position_; //!< The position of this transform component
      glm::quat rotation_; //!< The rotation of this transform component
      glm::vec3 euler_angles_; //!< Euler angles to avoid wrapping around
      glm::vec3 scale_; //!< The scale of this transform component
      glm::mat4x4 local_to_world_; //!< The local to world matrix
      glm::mat4x4 world_to_local_; //!< The world to local matrix

      /**
      * @brief The dirty flags to determine if this transform or its children
      *        should be updated
      */
      uint8_t is_dirty_;

      static const glm::vec3 kWorldUp_; //!< The up-axis in the world
      static const glm::vec3 kWorldForward_; //!< The forward-axis in the world
      static const glm::vec3 kWorldRight_; //!< The right-axis in the world
    };

    //--------------------------------------------------------------------------
    CREATE_COMPONENT(TransformComponent, Components::kTransform);
  }
}