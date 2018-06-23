#include "engine/components/mesh_component.h"

#ifndef SNUFF_NSCRIPTING
#include <sparsed/mesh_component.gen.cc>
#endif

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    MeshComponent::MeshComponent(Entity* entity) :
      ComponentBase<MeshComponent, Components::kMesh>(entity)
    {

    }

    //--------------------------------------------------------------------------
    void MeshComponent::Serialize(foundation::SaveArchive& archive) const
    {

    }

    //--------------------------------------------------------------------------
    void MeshComponent::Deserialize(foundation::LoadArchive& archive)
    {

    }
  }
}