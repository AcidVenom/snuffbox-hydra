#include "engine/components/camera_component.h"

#include <foundation/serialization/save_archive.h>
#include <foundation/serialization/load_archive.h>

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
      projection_(CameraProjection::kPerspective)
    {

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
    CameraProjection CameraComponent::projection() const
    {
      return projection_;
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