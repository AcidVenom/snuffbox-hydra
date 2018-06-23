#include "engine/assets/material_asset.h"
#include "engine/assets/shader_asset.h"
#include "engine/definitions/material_data.h"

#include "engine/application/application.h"
#include "engine/services/renderer_service.h"
#include "engine/services/asset_service.h"

#include <tools/compilers/compilers/material_compiler.h>

#include <foundation/auxiliary/logger.h>
#include <foundation/serialization/load_archive.h>

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    MaterialAsset::MaterialAsset(const foundation::Path& path) :
      IAsset(compilers::AssetTypes::kMaterial, path),
      vs_(nullptr),
      ps_(nullptr),
      gs_(nullptr),
      gpu_handle_(nullptr),
      renderer_(Application::Instance()->GetService<RendererService>()),
      asset_service_(Application::Instance()->GetService<AssetService>())
    {
      CreateGPUHandle();
    }

    //--------------------------------------------------------------------------
    void MaterialAsset::CreateGPUHandle()
    {
      gpu_handle_ = renderer_->GetLoader()->CreateMaterial();
    }

    //--------------------------------------------------------------------------
    bool MaterialAsset::LoadImpl(const foundation::Path& path)
    {
      vs_ = ps_ = gs_ = nullptr;

      compilers::MaterialCompiler c;
      if (c.Decompile(path) == false)
      {
        return false;
      }

      size_t len;
      const uint8_t* buffer = c.Data(&len);

      foundation::String src(reinterpret_cast<const char*>(buffer), len);

      foundation::LoadArchive archive;
      if (archive.FromJson(src) == false)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kEngine,
          foundation::LogSeverity::kError,
          "Could not load '{0}', an invalid JSON was provided",
          path);

        return false;
      }

      MaterialData data;
      archive(&data);

      vs_ = static_cast<ShaderAsset*>(asset_service_->Get(
        compilers::AssetTypes::kVertexShader, 
        data.vs_path));

      ps_ = static_cast<ShaderAsset*>(asset_service_->Get(
        compilers::AssetTypes::kPixelShader, 
        data.ps_path));

      if (
        vs_ == nullptr || 
        ps_ == nullptr ||
        vs_->Load() == false ||
        ps_->Load() == false)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kEngine,
          foundation::LogSeverity::kError,
          "Could not load '{0}', the vertex or pixel shader is invalid",
          path);

        return false;
      }

      if (data.has_gs == true)
      {
        gs_ = static_cast<ShaderAsset*>(asset_service_->Get(
          compilers::AssetTypes::kGeometryShader, 
          data.gs_path));

        if (gs_ == nullptr || gs_->Load() == false)
        {
          foundation::Logger::LogVerbosity<1>(
            foundation::LogChannel::kEngine,
            foundation::LogSeverity::kError,
            "Could not load '{0}', the requested geometry shader was invalid",
            path);

          return false;
        }
      }

      return renderer_->GetLoader()->LoadMaterial(
        gpu_handle_, 
        vs_->gpu_handle(), 
        ps_->gpu_handle(), 
        gs_ == nullptr ? nullptr : gs_->gpu_handle());
    }

    //--------------------------------------------------------------------------
    void MaterialAsset::UnloadImpl()
    {
      if (gpu_handle_ != nullptr)
      {
        renderer_->GetLoader()->UnloadMaterial(gpu_handle_);
      }
    }

    //--------------------------------------------------------------------------
    void MaterialAsset::Release()
    {
      if (gpu_handle_ != nullptr)
      {
        renderer_->GetLoader()->ReleaseMaterial(gpu_handle_);
      }
    }

    //--------------------------------------------------------------------------
    MaterialAsset::~MaterialAsset()
    {
      Release();
    }
  }
}