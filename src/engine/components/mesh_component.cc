#include "engine/components/mesh_component.h"
#include "engine/assets/model_asset.h"

#ifndef SNUFF_NSCRIPTING
#include <sparsed/mesh_component.gen.cc>
#endif

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    MeshComponent::MeshComponent(Entity* entity) :
      ComponentBase<MeshComponent, Components::kMesh>(entity),
      scene_index_(-1)
    {
      asset_.type = compilers::AssetTypes::kModel;
      asset_.handle = nullptr;
    }

    //--------------------------------------------------------------------------
    void MeshComponent::SetMesh(Mesh* mesh)
    {
      mesh_ = eastl::move(*mesh);
      scene_index_ = mesh_.index();

      asset_.handle = static_cast<IAsset*>(mesh_.asset());

      if (asset_.handle != nullptr)
      {
        asset_.name = asset_.handle->name();
      }
    }

    //--------------------------------------------------------------------------
    SerializableAsset* MeshComponent::asset()
    {
      return &asset_;
    }

    //--------------------------------------------------------------------------
    Mesh* MeshComponent::mesh()
    {
      return &mesh_;
    }

    //--------------------------------------------------------------------------
    int MeshComponent::scene_index() const
    {
      return scene_index_;
    }

    //--------------------------------------------------------------------------
    void MeshComponent::Serialize(foundation::SaveArchive& archive) const
    {
      archive(
        SET_ARCHIVE_PROP(asset_),
        SET_ARCHIVE_PROP(scene_index_));
    }

    //--------------------------------------------------------------------------
    void MeshComponent::Deserialize(foundation::LoadArchive& archive)
    {
      archive(
        GET_ARCHIVE_PROP(asset_),
        GET_ARCHIVE_PROP(scene_index_));

      if (asset_.handle != nullptr)
      {
        mesh_.FromModel(asset_.handle, scene_index_);
      }
    }
  }
}