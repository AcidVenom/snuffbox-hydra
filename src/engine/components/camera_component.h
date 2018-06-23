#pragma once

#include "engine/ecs/component.h"
#include "engine/definitions/camera.h"

namespace snuffbox
{
  namespace engine
  {
    /**
    * @brief Used to render cameras within the scene, both deferred and direct
    *
    * @author Daniel Konings
    */
    SCRIPT_CLASS() class CameraComponent :
      public ComponentBase<CameraComponent, Components::kCamera>
    {

    public:

      /**
      * @see IComponent::IComponent
      */
      CameraComponent(Entity* entity);

      SCRIPT_NAME(CameraComponent);

      /**
      * @brief Sets the near plane of the camera
      *
      * @param[in] near The near plane of the camera
      */
      SCRIPT_FUNC() void set_near(float near);

      /**
      * @brief Sets the far plane of the camera
      *
      * @param[in] far The far plane of the camera
      */
      SCRIPT_FUNC() void set_far(float far);

      /**
      * @brief Sets the projection mode of the camera
      *
      * @param[in] projection The projection mode of the camera
      */
      SCRIPT_FUNC() void set_projection(CameraProjection projection);

      /**
      * @return The near plane of the camera
      */
      SCRIPT_FUNC() float near() const;

      /**
      * @return The far plane of the camera
      */
      SCRIPT_FUNC() float far() const;

      /**
      * @return The projection mode of the camera
      */
      SCRIPT_FUNC() CameraProjection projection() const;

      /**
      * @see ISerializable::Serialize
      */
      void Serialize(foundation::SaveArchive& archive) const override;

      /**
      * @see ISerializable::Deserialize
      */
      void Deserialize(foundation::LoadArchive& archive) override;

    private:

      float near_; //!< The near plane of the camera
      float far_; //!< The far plane of the camera

      CameraProjection projection_; //!< The projection mode of the camera
    };

    //--------------------------------------------------------------------------
    CREATE_COMPONENT(CameraComponent, Components::kCamera);
  }
}