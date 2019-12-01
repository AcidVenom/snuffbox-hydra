#include "engine/graphics/material.h"

#include "engine/application/application.h"
#include "engine/services/asset_service.h"

#include "engine/assets/material_asset.h"

#ifndef SNUFF_NSCRIPTING
#include <sparsed/material.gen.cc>
#endif

namespace snuffbox
{
  namespace engine
  {
#ifndef SNUFF_NSCRIPTING
    //--------------------------------------------------------------------------
    Material::Material(scripting::ScriptArgs& args) :
      Material()
    {
      if (args.Check("O", true) == true)
      {
        SetMaterialAsset(args.GetPointer<IAsset>(0));
        return;
      }

      if (args.Check("S") == true)
      {
        SetMaterialAsset(args.Get<foundation::String>(0, "undefined"));
      }
    }
#endif

    //--------------------------------------------------------------------------
    Material::Material() :
      asset_(nullptr)
    {

    }

    //--------------------------------------------------------------------------
    Material::Material(IAsset* asset) :
      asset_(nullptr)
    {
      SetMaterialAsset(asset);
    }

    //--------------------------------------------------------------------------
    Material::Material(const foundation::String& name) :
      asset_(nullptr)
    {
      SetMaterialAsset(name);
    }

    //--------------------------------------------------------------------------
    void Material::SetMaterialAsset(IAsset* asset)
    {
      if (asset == nullptr)
      {
        asset_ = nullptr;
        return;
      }

      if (asset->type() != compilers::AssetTypes::kMaterial)
      {
        return;
      }

      asset_ = static_cast<MaterialAsset*>(asset);
    }

    //--------------------------------------------------------------------------
    void Material::SetMaterialAsset(const foundation::String& name)
    {
      AssetService* as = Application::Instance()->GetService<AssetService>();

      IAsset* asset = as->Get(compilers::AssetTypes::kMaterial, name);

      if (asset == nullptr)
      {
        return;
      }

      SetMaterialAsset(asset);
    }

    //--------------------------------------------------------------------------
    MaterialAsset* Material::asset() const
    {
      return asset_;
    }

    //--------------------------------------------------------------------------
    SerializableAsset Material::GetSerializableAsset() const
    {
      SerializableAsset sa;
      sa.handle = asset_;
      sa.type = compilers::AssetTypes::kMaterial;

      if (asset_ != nullptr)
      {
        sa.SetName(asset_->name());
      }

      return sa;
    }
  }
}