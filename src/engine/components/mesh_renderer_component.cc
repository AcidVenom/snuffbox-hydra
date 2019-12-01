#include "engine/components/mesh_renderer_component.h"
#include "engine/components/mesh_component.h"
#include "engine/components/transform_component.h"

#include "engine/ecs/entity.h"

#include "engine/assets/material_asset.h"
#include "engine/services/renderer_service.h"

#include "engine/application/application.h"
#include "engine/services/asset_service.h"

#include <foundation/serialization/save_archive.h>
#include <foundation/serialization/load_archive.h>

#ifndef SNUFF_NSCRIPTING
#include <sparsed/mesh_renderer_component.gen.cc>
#endif

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    const int MeshRendererComponent::kMaxMaterials = 8;

    //--------------------------------------------------------------------------
    MeshRendererComponent::MeshRendererComponent(Entity* entity) :
      ComponentBase<MeshRendererComponent, Components::kMeshRenderer>(entity),
      renderer_(Application::Instance()->GetService<RendererService>())
    {
      shared_materials_.resize(kMaxMaterials);
      
      for (size_t i = 0; i < shared_materials_.size(); ++i)
      {
        SerializableAsset& shared = shared_materials_.at(i);

        shared.type = compilers::AssetTypes::kMaterial;
        shared.handle = nullptr;
      }

      materials_.resize(shared_materials_.size());
    }

    //--------------------------------------------------------------------------
    void MeshRendererComponent::Update(float dt)
    {
      if (shared_materials_.size() == 0)
      {
        return;
      }

      MeshComponent* m = entity()->GetComponent<MeshComponent>();
      TransformComponent* t = entity()->GetComponent<TransformComponent>();

      renderer_->Queue(m, t, this);
    }

    //--------------------------------------------------------------------------
    void MeshRendererComponent::SetMaterial(
      int idx, 
      Material* material)
    {
      if (material == nullptr)
      {
        return;
      }

      if (idx >= kMaxMaterials || idx < 0)
      {
        return;
      }

      SerializableAsset& sa = shared_materials_.at(idx);
      sa.handle = material->asset();

      if (sa.handle != nullptr)
      {
        sa.SetName(sa.handle->name());
      }

      materials_.at(idx) = *material;
    }

    //--------------------------------------------------------------------------
    Material* MeshRendererComponent::GetMaterial(int idx)
    {
      if (idx >= kMaxMaterials || idx < 0)
      {
        return nullptr;
      }

      return &materials_.at(idx);
    }

    //--------------------------------------------------------------------------
    foundation::Vector<SerializableAsset>& 
      MeshRendererComponent::shared_materials()
    {
      return shared_materials_;
    }

    //--------------------------------------------------------------------------
    void MeshRendererComponent::Serialize(
      foundation::SaveArchive& archive) const
    {
      archive(SET_ARCHIVE_PROP(shared_materials_));
    }

    //--------------------------------------------------------------------------
    void MeshRendererComponent::Deserialize(foundation::LoadArchive& archive)
    {
      archive(GET_ARCHIVE_PROP(shared_materials_));

      for (size_t i = 0; i < shared_materials_.size(); ++i)
      {
        SerializableAsset& a = shared_materials_.at(i);

        if (a.handle != nullptr)
        {
          materials_.at(i) = Material(a.handle);
        }
      }
    }
  }
}