#include "engine/assets/scene_asset.h"

#include "engine/application/application.h"
#include "engine/ecs/scene.h"

#include <tools/compilers/scene_compiler.h>

#include <foundation/serialization/load_archive.h>

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    SceneAsset::SceneAsset(const foundation::Path& path) :
      IAsset(compilers::AssetTypes::kScene, path),
      scene_(nullptr)
    {
      scene_ = foundation::Memory::Construct<Scene>(
        &foundation::Memory::default_allocator());
    }

    //--------------------------------------------------------------------------
    Scene* SceneAsset::scene() const
    {
      return scene_;
    }

    //--------------------------------------------------------------------------
    bool SceneAsset::LoadImpl(const foundation::Path& path)
    {
      compilers::SceneCompiler c;
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

      archive(&scene_);

      return true;
    }

    //--------------------------------------------------------------------------
    void SceneAsset::UnloadImpl()
    {
      scene_->Clear();
    }

    //--------------------------------------------------------------------------
    SceneAsset::~SceneAsset()
    {
      if (scene_ != nullptr)
      {
        foundation::Memory::Destruct(scene_);
        scene_ = nullptr;
      }
    }
  }
}