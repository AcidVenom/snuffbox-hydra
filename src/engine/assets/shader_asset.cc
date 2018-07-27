#include "engine/assets/shader_asset.h"
#include "engine/application/application.h"
#include "engine/services/renderer_service.h"

#include <tools/compilers/compilers/shader_compiler.h>

#include <graphics/definitions/shader_types.h>

#include <foundation/auxiliary/logger.h>

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    ShaderAsset::ShaderAsset(
      const foundation::Path& path,
      const foundation::String& name,
      compilers::AssetTypes type)
      :
      IAsset(type, path, name),
      gpu_handle_(nullptr),
      renderer_(Application::Instance()->GetService<RendererService>())
    {
      CreateGPUHandle();
    }

    //--------------------------------------------------------------------------
    void ShaderAsset::CreateGPUHandle()
    {
      compilers::AssetTypes t = type();
      graphics::ShaderTypes shader = graphics::ShaderTypes::kUnknown;

      switch (t)
      {
      case compilers::AssetTypes::kVertexShader:
        shader = graphics::ShaderTypes::kVertex;
        break;

      case compilers::AssetTypes::kPixelShader:
        shader = graphics::ShaderTypes::kPixel;
        break;

      case compilers::AssetTypes::kGeometryShader:
        shader = graphics::ShaderTypes::kGeometry;
        break;

      default:
        break;
      }

      if (shader == graphics::ShaderTypes::kUnknown)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kEngine,
          foundation::LogSeverity::kError,
          "Could not load shader '{0}',\
          it was of an invalid shader type ({1})",
          path(),
          AssetTypesToString(t));

        return;
      }

      gpu_handle_ = renderer_->GetLoader()->CreateShader(shader);
    }

    //--------------------------------------------------------------------------
    bool ShaderAsset::LoadImpl(const foundation::Path& path)
    {
      compilers::AssetTypes t = type();
      compilers::ShaderCompiler c(t);

      if (c.Decompile(path) == false)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kEngine,
          foundation::LogSeverity::kError,
          "Could not decompile shader '{0}', errors:\n{1}",
          path,
          c.error());

        return false;
      }

      size_t len;
      const uint8_t* buffer = c.Data(&len);

      return renderer_->GetLoader()->LoadShader(gpu_handle_, buffer, len);
    }

    //--------------------------------------------------------------------------
    void ShaderAsset::UnloadImpl()
    {
      if (gpu_handle_ == nullptr)
      {
        return;
      }

      renderer_->GetLoader()->UnloadShader(gpu_handle_);
    }

    //--------------------------------------------------------------------------
    void* ShaderAsset::gpu_handle() const
    {
      return gpu_handle_;
    }

    //--------------------------------------------------------------------------
    ShaderAsset::~ShaderAsset()
    {
      renderer_->GetLoader()->ReleaseShader(gpu_handle_);
    }
  }
}