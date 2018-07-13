#include "engine/assets/model_asset.h"
#include "engine/application/application.h"
#include "engine/services/renderer_service.h"

#include <tools/compilers/compilers/model_compiler.h>

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    ModelAsset::ModelAsset(const foundation::Path& path) :
      IAsset(compilers::AssetTypes::kModel, path),
      renderer_(Application::Instance()->GetService<RendererService>())
    {

    }

    //--------------------------------------------------------------------------
    bool ModelAsset::IsValid(int scene_index) const
    {
      if (scene_index < 0 || scene_index >= meshes_.size())
      {
        return false;
      }

      return meshes_.at(scene_index) != nullptr;
    }

    //--------------------------------------------------------------------------
    void* ModelAsset::GetGPUHandle(int scene_index) const
    {
      if (IsValid(scene_index) == false)
      {
        return nullptr;
      }

      return meshes_.at(scene_index);
    }

    //--------------------------------------------------------------------------
    bool ModelAsset::LoadImpl(const foundation::Path& path)
    {
      using Compiler = compilers::ModelCompiler<graphics::Vertex3D>;

      Compiler compiler;
      if (compiler.Decompile(path) == false)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kEngine,
          foundation::LogSeverity::kError,
          "Could not load model '{0}', errors:\n{1}",
          path,
          compiler.error());

        return false;
      }

      const foundation::Vector<Compiler::Mesh>& meshes = compiler.meshes();

      size_t n = meshes.size();
      meshes_.resize(n);
      void* handle = nullptr;

      graphics::IRendererLoader* loader = renderer_->GetLoader();

      for (size_t i = 0; i < n; ++i)
      {
        const Compiler::Mesh& mesh = meshes.at(i);

        handle = loader->CreateMesh();
        
        if (loader->LoadMesh(handle, mesh.vertices, mesh.indices) == false)
        {
          foundation::Logger::LogVerbosity<1>(
            foundation::LogChannel::kEngine,
            foundation::LogSeverity::kWarning,
            "Could not load mesh {0} of model '{1}', it will be invalid",
            i,
            path);

          loader->ReleaseMesh(handle);
          handle = nullptr;
        }

        meshes_.at(i) = handle;
      }

      return true;
    }

    //--------------------------------------------------------------------------
    void ModelAsset::UnloadImpl()
    {
      Release();
    }

    //--------------------------------------------------------------------------
    void ModelAsset::Release()
    {
      void* handle = nullptr;
      for (size_t i = 0; i < meshes_.size(); ++i)
      {
        handle = meshes_.at(i);

        if (handle != nullptr)
        {
          renderer_->GetLoader()->ReleaseMesh(handle);
          meshes_.at(i) = nullptr;
        }
      }
    }
  }
}