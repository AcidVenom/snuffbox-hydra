#pragma once

#include "engine/ecs/component.h"
#include "engine/definitions/camera.h"

#include <glm/glm.hpp>

#ifdef near
#undef near
#endif

#ifdef far
#undef far
#endif

namespace snuffbox
{
  namespace engine
  {
    class RendererService;

    /**
    * @brief Used to render cameras within the scene, both deferred and direct
    *
    * @author Daniel Konings
    */
    SCRIPT_CLASS() class CameraComponent :
      public ComponentBase<CameraComponent, Components::kCamera>
    {

    public:

      SCRIPT_NAME(CameraComponent);

      /**
      * @see IComponent::IComponent
      */
      CameraComponent(Entity* entity);

      /**
      * @see IComponent::Update
      *
      * Updates the camera component to send a camera queue event to the
      * RendererService
      */
      void Update(float dt) override;

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
      * @brief Sets the field of view of the camera, in degrees
      *
      * @param[in] fov The field of view to set
      */
      SCRIPT_FUNC() void set_fov(float fov);

      /**
      * @brief Sets the aspect ratio of the camera
      *
      * @param[in] aspect The aspect ratio to set
      */
      SCRIPT_FUNC() void set_aspect(float aspect);

      /**
      * @brief Sets the orthographic size of the camera
      *
      * @param[in] size The size to set
      */
      SCRIPT_FUNC() void set_orthographic_size(float size);

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
      * @return The field of view of the camera
      */
      SCRIPT_FUNC() float fov() const;

      /**
      * @return The aspect ratio of the camera
      */
      SCRIPT_FUNC() float aspect() const;

      /**
      * @return The orthographic siez of the camera
      */
      SCRIPT_FUNC() float orthographic_size() const;

      /**
      * @return The projection mode of the camera
      */
      SCRIPT_FUNC() CameraProjection projection() const;

      /**
      * @return The camera's projection matrix 
      */
      const glm::mat4x4& projection_matrix() const;

      /**
      * @return The camera's view matrix
      */
      const glm::mat4x4& view_matrix() const;

    protected:

      /**
      * @brief Updates the camera's view and projection matrices from its
      *        properties
      */
      void UpdateMatrices();

    public:

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
      float fov_; //!< The field of view of the camera
      float aspect_; //!< The aspect ratio of the camera
      float orthographic_size_; //!< The orthographic size of the camera

      CameraProjection projection_; //!< The projection mode of the camera

      glm::mat4x4 projection_matrix_; //!< The camera's projection matrix
      glm::mat4x4 view_matrix_; //!< The camera's view matrix

      RendererService* renderer_; //!< The renderer service
    };

    //--------------------------------------------------------------------------
    CREATE_COMPONENT(CameraComponent, Components::kCamera);
  }
}