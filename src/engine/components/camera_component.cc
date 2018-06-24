#include "engine/components/camera_component.h"
#include "engine/services/renderer_service.h"

#include <foundation/serialization/save_archive.h>
#include <foundation/serialization/load_archive.h>

#ifdef near
#undef near
#endif

#ifdef far
#undef far
#endif

#include <glm/gtc/matrix_transform.hpp>

#ifndef SNUFF_NSCRIPTING
#include <sparsed/camera_component.gen.cc>
#endif

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    CameraComponent::CameraComponent(Entity* entity) :
      ComponentBase<CameraComponent, Components::kCamera>(entity),
      near_(0.1f),
      far_(100.0f),
      fov_(90.0f),
      aspect_(16.0f / 9.0f),
      projection_(CameraProjection::kPerspective),
      renderer_(Application::Instance()->GetService<RendererService>())
    {

    }

    //--------------------------------------------------------------------------
    void CameraComponent::Update(float dt)
    {
      UpdateMatrices();
      renderer_->Queue(entity()->GetComponent<TransformComponent>(), this);
    }

    //--------------------------------------------------------------------------
    void CameraComponent::set_near(float near)
    {
      near_ = near;
    }

    //--------------------------------------------------------------------------
    void CameraComponent::set_far(float far)
    {
      far_ = far;
    }

    //--------------------------------------------------------------------------
    void CameraComponent::set_fov(float fov)
    {
      fov_ = fov;
    }

    //--------------------------------------------------------------------------
    void CameraComponent::set_aspect(float aspect)
    {
      aspect_ = aspect;
    }

    //--------------------------------------------------------------------------
    void CameraComponent::set_projection(CameraProjection projection)
    {
      projection_ = projection;
    }

    //--------------------------------------------------------------------------
    float CameraComponent::near() const
    {
      return near_;
    }

    //--------------------------------------------------------------------------
    float CameraComponent::far() const
    {
      return far_;
    }

    //--------------------------------------------------------------------------
    float CameraComponent::fov() const
    {
      return fov_;
    }

    //--------------------------------------------------------------------------
    float CameraComponent::aspect() const
    {
      return aspect_;
    }

    //--------------------------------------------------------------------------
    CameraProjection CameraComponent::projection() const
    {
      return projection_;
    }

    //--------------------------------------------------------------------------
    const glm::mat4x4& CameraComponent::projection_matrix() const
    {
      return projection_matrix_;
    }

    //--------------------------------------------------------------------------
    const glm::mat4x4& CameraComponent::view_matrix() const
    {
      return view_matrix_;
    }

    //--------------------------------------------------------------------------
    void CameraComponent::UpdateMatrices()
    {
      projection_matrix_ = 
        glm::perspectiveLH_ZO(glm::radians(fov_), aspect_, near_, far_);

      TransformComponent* t = entity()->GetComponent<TransformComponent>();
      const glm::vec3 eye = t->GetPosition();

      view_matrix_ =
        glm::lookAtLH(eye, eye + t->Forward(), t->Up());
    }

    //--------------------------------------------------------------------------
    void CameraComponent::Serialize(foundation::SaveArchive& archive) const
    {
      archive(
        SET_ARCHIVE_PROP(near_),
        SET_ARCHIVE_PROP(far_),
        SET_ARCHIVE_PROP(projection_));
    }

    //--------------------------------------------------------------------------
    void CameraComponent::Deserialize(foundation::LoadArchive& archive)
    {
      archive(
        GET_ARCHIVE_PROP(near_),
        GET_ARCHIVE_PROP(far_),
        GET_ARCHIVE_PROP(projection_));
    }
  }
}