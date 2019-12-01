#include "engine/services/asset_service.h"

#include "engine/assets/script_asset.h"
#include "engine/assets/scene_asset.h"
#include "engine/assets/shader_asset.h"
#include "engine/assets/material_asset.h"
#include "engine/assets/model_asset.h"

#include <foundation/io/directory_tree.h>
#include <foundation/auxiliary/logger.h>

#ifndef SNUFF_NSCRIPTING
#include <sparsed/asset_service.gen.cc>
#endif

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    AssetService::AssetService() :
      ServiceBase<AssetService>("AssetService")
    {

    }

    //--------------------------------------------------------------------------
    foundation::ErrorCodes AssetService::OnInitialize(Application& app)
    {
      return foundation::ErrorCodes::kSuccess;
    }

    //--------------------------------------------------------------------------
    void AssetService::OnShutdown(Application& app)
    {
      Clear();
    }

    //--------------------------------------------------------------------------
    foundation::Vector<AssetService::AssetFile> AssetService::EnumerateAssets(
      const foundation::Path& dir,
      const foundation::Path& root,
      bool recursive,
      bool include_directories)
    {
      foundation::Vector<AssetFile> result;
      foundation::DirectoryTree tree(dir);

      const foundation::Vector<foundation::DirectoryTreeItem>& items =
        tree.items();

      AssetFile af;
      foundation::String ext;

      for (size_t i = 0; i < items.size(); ++i)
      {
        const foundation::DirectoryTreeItem& item = items.at(i);
        const foundation::Path& item_path = item.path();
        if (IsAsset(item_path) == true)
        {
          ext = item_path.extension();

          af.relative_path = item_path.StripPath(root);
          af.type = compilers::AssetTypesFromBuildExtension(ext.c_str());
          af.is_directory = false;

          result.push_back(af);
          continue;
        }

        if (include_directories == true && item.is_directory() == true)
        {
          af.relative_path = item_path.StripPath(root);
          af.type = compilers::AssetTypes::kCount;
          af.is_directory = true;

          result.push_back(af);
        }

        if (recursive == true && item.is_directory() == true)
        {
          foundation::Vector<AssetFile> children =
            EnumerateAssets(item.path(), root, recursive, include_directories);

          for (size_t j = 0; j < children.size(); ++j)
          {
            result.push_back(children.at(j));
          }
        }
      }

      return result;
    }

    //--------------------------------------------------------------------------
    void AssetService::Refresh(const foundation::Path& path)
    {
      Clear();
      build_directory_ = path;

      foundation::Vector<AssetFile> files = EnumerateAssets(path, path, true);

      for (size_t i = 0; i < files.size(); ++i)
      {
        const AssetFile& file = files.at(i);
        RegisterAsset(file.type, file.relative_path);
      }
    }

    //--------------------------------------------------------------------------
    void AssetService::Clear()
    {
      AssetMap::iterator it = registered_.begin();

      while (it != registered_.end())
      {
        it->second->Unload();
        ++it;
      }

      registered_.clear();
    }

    //--------------------------------------------------------------------------
    bool AssetService::Load(
      compilers::AssetTypes type, 
      const foundation::String& path)
    {
      if (Exists(type, path) == false)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kEngine,
          foundation::LogSeverity::kError,
          "Could not load asset '{0}' of type '{1}', it does not exist",
          path,
          compilers::AssetTypesToString(type));

        return false;
      }

      foundation::String ext = compilers::AssetTypesToBuildExtension(type);
      AssetMap::iterator it = registered_.find(path + '.' + ext);

      if (it->second->Load() == false)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kEngine,
          foundation::LogSeverity::kError,
          "Could not load asset '{0}' of type '{1}', loading failed",
          path,
          compilers::AssetTypesToString(type));

        return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------
    bool AssetService::Reload(
      compilers::AssetTypes type,
      const foundation::String& path)
    {
      if (IsLoaded(type, path) == true)
      {
        return Load(type, path);
      }

      return false;
    }

    //--------------------------------------------------------------------------
    void AssetService::Unload(
      compilers::AssetTypes type,
      const foundation::String& path)
    {
      IAsset* ptr = Get(type, path);

      if (ptr == nullptr || ptr->is_loaded() == false)
      {
        return;
      }

      ptr->Unload();
    }

    //--------------------------------------------------------------------------
    bool AssetService::LoadAll(compilers::AssetTypes type)
    {
      AssetMap::iterator it = registered_.begin();

      bool success = true;
      foundation::SharedPtr<IAsset> a;

      while (it != registered_.end())
      {
        a = it->second;
        if (a->type() == type)
        {
          if (
            Load(type, foundation::Path(it->first).NoExtension().ToString())
            == false)
          {
            success = false;
          }
        }

        ++it;
      }

      return success;
    }

    //--------------------------------------------------------------------------
    void AssetService::UnloadAll(compilers::AssetTypes type)
    {
      AssetMap::iterator it = registered_.begin();
      foundation::SharedPtr<IAsset> a;

      while (it != registered_.end())
      {
        a = it->second;

        if (a->type() == type && a->is_loaded() == true)
        {
          a->Unload();
        }

        ++it;
      }
    }

    //--------------------------------------------------------------------------
    bool AssetService::IsAsset(const foundation::Path& path)
    {
      if (path.is_directory() == true)
      {
        return false;
      }

      foundation::String ext = path.extension();

      return compilers::AssetTypesFromBuildExtension(ext.c_str()) !=
        compilers::AssetTypes::kCount;
    }

    //--------------------------------------------------------------------------
    foundation::String AssetService::NoExtensionToBuildPath(
      compilers::AssetTypes type,
      const foundation::String& path)
    {
      return path + '.' + compilers::AssetTypesToBuildExtension(type);
    }

    //--------------------------------------------------------------------------
    void AssetService::RegisterAsset(const foundation::Path& path)
    {
      foundation::String ext = path.extension();

      compilers::AssetTypes type = 
        compilers::AssetTypesFromBuildExtension(ext.c_str());

      RegisterAsset(type, path);
    }

    //--------------------------------------------------------------------------
    void AssetService::RegisterAsset(
      compilers::AssetTypes type,
      const foundation::Path& relative_path)
    {
      std::unique_lock<std::mutex> lock(mutex_);

      foundation::Path no_ext = relative_path.NoExtension();
      foundation::String no_ext_s = no_ext.ToString();

      if (Exists(type, no_ext_s) == true)
      {
        return;
      }

      if (
        type == compilers::AssetTypes::kCount)
      {
        return;
      }

      IAsset* ptr = nullptr;
      foundation::IAllocator* alloc = &foundation::Memory::default_allocator();
      foundation::Path path = build_directory_ / relative_path;

      switch (type)
      {
      case compilers::AssetTypes::kScript:
        ptr = foundation::Memory::Construct<ScriptAsset>(alloc, path, no_ext_s);
        break;

      case compilers::AssetTypes::kScene:
        ptr = foundation::Memory::Construct<SceneAsset>(alloc, path, no_ext_s);
        break;

      case compilers::AssetTypes::kVertexShader:
      case compilers::AssetTypes::kPixelShader:
      case compilers::AssetTypes::kGeometryShader:
        ptr = foundation::Memory::Construct<ShaderAsset>(
          alloc, path, no_ext_s, type);
        break;

      case compilers::AssetTypes::kMaterial:
        ptr = foundation::Memory::Construct<MaterialAsset>(
          alloc, path, no_ext_s);
        break;

      case compilers::AssetTypes::kModel:
        ptr = foundation::Memory::Construct<ModelAsset>(alloc, path, no_ext_s);
        break;

      default:
        foundation::Logger::Assert(false,
          "Attempted to register an unknown asset type");
        break;
      }

      if (ptr == nullptr)
      {
        return;
      }

      registered_.emplace(
        eastl::pair<foundation::String, foundation::SharedPtr<IAsset>>
      {
        relative_path.ToString(), foundation::Memory::MakeShared(ptr)
      });
    }

    //--------------------------------------------------------------------------
    void AssetService::RemoveAsset(
      compilers::AssetTypes type,
      const foundation::Path& relative_path)
    {
      std::unique_lock<std::mutex> lock(mutex_);

      AssetMap::iterator it = registered_.find(relative_path.ToString());

      if (it != registered_.end())
      {
        IAsset* asset = it->second.get();
        if (asset->is_loaded() == true)
        {
          asset->Unload();
        }

        registered_.erase(it);
      }
    }

    //--------------------------------------------------------------------------
    bool AssetService::Exists(
      compilers::AssetTypes type,
      const foundation::String& path) const
    {
      foundation::String p = NoExtensionToBuildPath(type, path);

      return registered_.find(p) != registered_.end();
    }

    //--------------------------------------------------------------------------
    bool AssetService::IsLoaded(
      compilers::AssetTypes type,
      const foundation::String& path) const
    {
      foundation::String p = NoExtensionToBuildPath(type, path);

      return Exists(type, path) == true && registered_.at(p)->is_loaded();
    }

    //--------------------------------------------------------------------------
    IAsset* AssetService::Get(
      compilers::AssetTypes type,
      const foundation::String& path)
    {
      if (Exists(type, path) == false)
      {
        return nullptr;
      }

      foundation::String p = NoExtensionToBuildPath(type, path);
      return registered_.at(p).get();
    }

    //--------------------------------------------------------------------------
    foundation::Vector<const IAsset*> AssetService::GetRegistered(
      compilers::AssetTypes type) const
    {
      foundation::Vector<const IAsset*> result;

      for (
        AssetMap::const_iterator it = registered_.begin();
        it != registered_.end();
        ++it)
      {
        const IAsset* asset = it->second.get();
        if (asset->type() == type)
        {
          result.push_back(asset);
        }
      }

      return result;
    }

    //--------------------------------------------------------------------------
    void AssetService::set_build_directory(const foundation::Path& path)
    {
      build_directory_ = path;
    }

    //--------------------------------------------------------------------------
    IAsset* AssetService::Get(int type, const foundation::String& path)
    {
      compilers::AssetTypes t = static_cast<compilers::AssetTypes>(type);
      if (
        t < compilers::AssetTypes::kScript || 
        t >= compilers::AssetTypes::kCount)
      {
        return nullptr;
      }

      return Get(t, path);
    }

    //--------------------------------------------------------------------------
    bool AssetService::Exists(int type, const foundation::String& path)
    {
      compilers::AssetTypes t = static_cast<compilers::AssetTypes>(type);
      if (
        t < compilers::AssetTypes::kScript || 
        t >= compilers::AssetTypes::kCount)
      {
        return false;
      }

      return Exists(t, path);
    }

    //--------------------------------------------------------------------------
    bool AssetService::Load(int type, const foundation::String& path)
    {
      compilers::AssetTypes t = static_cast<compilers::AssetTypes>(type);
      if (
        t < compilers::AssetTypes::kScript || 
        t >= compilers::AssetTypes::kCount)
      {
        return false;
      }

      return Load(t, path);
    }

    //--------------------------------------------------------------------------
    void AssetService::Unload(int type, const foundation::String& path)
    {
      compilers::AssetTypes t = static_cast<compilers::AssetTypes>(type);
      if (
        t < compilers::AssetTypes::kScript || 
        t >= compilers::AssetTypes::kCount)
      {
        return;
      }

      return Unload(t, path);
    }
  }
}