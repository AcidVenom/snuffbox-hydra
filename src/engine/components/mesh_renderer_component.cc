#include "engine/components/mesh_renderer_component.h"
#include "engine/assets/material_asset.h"

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
    MeshRendererComponent::MeshRendererComponent(Entity* entity) :
      ComponentBase<MeshRendererComponent, Components::kMeshRenderer>(entity),
      material_path_(""),
      material_(nullptr)
    {

    }

    //--------------------------------------------------------------------------
    void MeshRendererComponent::SetMaterial(const foundation::String& path)
    {
      AssetService* as = Application::Instance()->GetService<AssetService>();

      material_path_ = path;

      material_ = static_cast<MaterialAsset*>(as->Get(
        compilers::AssetTypes::kMaterial, 
        path));
    }

    //--------------------------------------------------------------------------
    const foundation::String& MeshRendererComponent::material_path() const
    {
      return material_path_;
    }

    //--------------------------------------------------------------------------
    MaterialAsset* MeshRendererComponent::material() const
    {
      return material_;
    }

    //--------------------------------------------------------------------------
    void MeshRendererComponent::Serialize(
      foundation::SaveArchive& archive) const
    {
      archive(SET_ARCHIVE_PROP(material_path_));
    }

    //--------------------------------------------------------------------------
    void MeshRendererComponent::Deserialize(foundation::LoadArchive& archive)
    {
      archive(GET_ARCHIVE_PROP(material_path_));

      SetMaterial(material_path_);
    }
  }
}