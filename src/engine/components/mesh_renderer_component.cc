#include "engine/components/mesh_renderer_component.h"

#ifndef SNUFF_NSCRIPTING
#include <sparsed/mesh_renderer_component.gen.cc>
#endif

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    MeshRendererComponent::MeshRendererComponent(Entity* entity) :
      ComponentBase<MeshRendererComponent, Components::kMeshRenderer>(entity)
    {

    }

    //--------------------------------------------------------------------------
    void MeshRendererComponent::Serialize(
      foundation::SaveArchive& archive) const
    {

    }

    //--------------------------------------------------------------------------
    void MeshRendererComponent::Deserialize(foundation::LoadArchive& archive)
    {

    }
  }
}