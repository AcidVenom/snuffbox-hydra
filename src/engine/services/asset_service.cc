#include "engine/services/asset_service.h"

#include "engine/assets/script_asset.h"
#include "engine/assets/scene_asset.h"

#include <foundation/io/directory_tree.h>
#include <foundation/auxiliary/logger.h>

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
      bool recursive)
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

          result.push_back(af);
          continue;
        }

        if (recursive == true && item.is_directory() == true)
        {
          foundation::Vector<AssetFile> children =
            EnumerateAssets(item.path(), root, recursive);

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
    bool AssetService::LoadAll(compilers::AssetTypes type)
    {
      AssetMap::iterator it = registered_.begin();

      bool success = true;
      while (it != registered_.end())
      {
        foundation::SharedPtr<IAsset>& a = it->second;
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
    bool AssetService::IsAsset(const foundation::Path& path)
    {
      if (path.is_directory() == true)
      {
        return false;
      }

      foundation::String ext = path.extension();

      return compilers::AssetTypesFromBuildExtension(ext.c_str()) !=
        compilers::AssetTypes::kUnknown;
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
      foundation::Path no_ext = relative_path.NoExtension();
      foundation::String no_ext_s = no_ext.ToString();

      if (Exists(type, no_ext_s) == true)
      {
        return;
      }

      if (
        type == compilers::AssetTypes::kCount ||
        type == compilers::AssetTypes::kUnknown)
      {
        return;
      }

      IAsset* ptr = nullptr;
      foundation::IAllocator* alloc = &foundation::Memory::default_allocator();
      foundation::Path path = build_directory_ / relative_path;

      switch (type)
      {
      case compilers::AssetTypes::kScript:
        ptr = foundation::Memory::Construct<ScriptAsset>(alloc, path);
        break;

      case compilers::AssetTypes::kScene:
        ptr = foundation::Memory::Construct<SceneAsset>(alloc, path);
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
    void AssetService::set_build_directory(const foundation::Path& path)
    {
      build_directory_ = path;
    }
  }
}