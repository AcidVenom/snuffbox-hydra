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
    const int MeshRendererComponent::kMaxMaterials_ = 8;

    //--------------------------------------------------------------------------
    MeshRendererComponent::MeshRendererComponent(Entity* entity) :
      ComponentBase<MeshRendererComponent, Components::kMeshRenderer>(entity),
      renderer_(Application::Instance()->GetService<RendererService>())
    {
      materials_.resize(kMaxMaterials_);
      
      for (size_t i = 0; i < materials_.size(); ++i)
      {
        materials_.at(i).type = compilers::AssetTypes::kMaterial;
        materials_.at(i).handle = nullptr;
      }
    }

    //--------------------------------------------------------------------------
    void MeshRendererComponent::Update(float dt)
    {
      if (materials_.size() == 0)
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
      IAsset* material)
    {
      if (material->type() != compilers::AssetTypes::kMaterial)
      {
        return;
      }

      if (idx >= kMaxMaterials_ || idx < 0)
      {
        return;
      }

      materials_.at(idx).handle = material;
    }

    //--------------------------------------------------------------------------
    IAsset* MeshRendererComponent::GetMaterial(int idx)
    {
      if (idx >= kMaxMaterials_ || idx < 0)
      {
        return nullptr;
      }

      return materials_.at(idx).handle;
    }

    //--------------------------------------------------------------------------
    foundation::Vector<SerializableAsset>& MeshRendererComponent::materials()
    {
      return materials_;
    }

    //--------------------------------------------------------------------------
    void MeshRendererComponent::Serialize(
      foundation::SaveArchive& archive) const
    {
      archive(SET_ARCHIVE_PROP(materials_));
    }

    //--------------------------------------------------------------------------
    void MeshRendererComponent::Deserialize(foundation::LoadArchive& archive)
    {
      archive(GET_ARCHIVE_PROP(materials_));
    }
  }
}