#include "engine/assets/asset_manager.h"

#include "engine/assets/script_asset.h"

#include <foundation/io/directory_tree.h>
#include <foundation/auxiliary/logger.h>

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    foundation::Vector<AssetManager::AssetFile> AssetManager::EnumerateAssets(
      const foundation::Path& path,
      bool recursive)
    {
      foundation::Vector<AssetFile> result;
      foundation::DirectoryTree tree(path);

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

          af.relative_path = item_path.StripPath(path);
          af.type = builder::AssetTypesFromBuildExtension(ext.c_str());

          result.push_back(af);
          continue;
        }

        if (recursive == true && item.is_directory() == true)
        {
          foundation::Vector<AssetFile> children =
            EnumerateAssets(item.path(), recursive);

          for (size_t j = 0; j < children.size(); ++j)
          {
            result.push_back(children.at(j));
          }
        }
      }

      return result;
    }

    //--------------------------------------------------------------------------
    void AssetManager::Clear()
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
    bool AssetManager::Load(
      builder::AssetTypes type, 
      const foundation::String& path)
    {
      if (Exists(type, path) == false)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kEngine,
          foundation::LogSeverity::kError,
          "Could not load asset '{0}' of type '{1}', it does not exist",
          path,
          builder::AssetTypesToString(type));

        return false;
      }

      foundation::String ext = builder::AssetTypesToBuildExtension(type);
      AssetMap::iterator it = registered_.find(path + '.' + ext);
      
      if (it->second->Load() == false)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kEngine,
          foundation::LogSeverity::kError,
          "Could not load asset '{0}' of type '{1}', loading failed",
          path,
          builder::AssetTypesToString(type));

        return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------
    void AssetManager::LoadAll(builder::AssetTypes type)
    {
      AssetMap::iterator it = registered_.begin();
      
      while (it != registered_.end())
      {
        foundation::SharedPtr<IAsset>& a = it->second;
        if (a->type() == type)
        {
          Load(type, foundation::Path(it->first).NoExtension().ToString());
        }

        ++it;
      }
    }

    //--------------------------------------------------------------------------
    bool AssetManager::IsAsset(const foundation::Path& path)
    {
      if (path.is_directory() == true)
      {
        return false;
      }

      foundation::String ext = path.extension();

      return builder::AssetTypesFromBuildExtension(ext.c_str()) !=
        builder::AssetTypes::kUnknown;
    }

    //--------------------------------------------------------------------------
    foundation::String AssetManager::NoExtensionToBuildPath(
      builder::AssetTypes type,
      const foundation::String& path)
    {
      return path + '.' + builder::AssetTypesToBuildExtension(type);
    }

    //--------------------------------------------------------------------------
    void AssetManager::RegisterAsset(const foundation::Path& path)
    {
      foundation::String ext = path.extension();

      builder::AssetTypes type = 
        builder::AssetTypesFromBuildExtension(ext.c_str());

      RegisterAsset(type, path);
    }

    //--------------------------------------------------------------------------
    void AssetManager::RegisterAsset(
      builder::AssetTypes type,
      const foundation::Path& relative_path)
    {
      foundation::Path no_ext = relative_path.NoExtension();
      foundation::String no_ext_s = no_ext.ToString();

      if (Exists(type, no_ext_s) == true)
      {
        if (IsLoaded(type, no_ext_s) == true)
        {
          Load(type, no_ext_s);
        }

        return;
      }

      if (
        type == builder::AssetTypes::kCount ||
        type == builder::AssetTypes::kUnknown)
      {
        return;
      }

      IAsset* ptr = nullptr;
      foundation::IAllocator* alloc = &foundation::Memory::default_allocator();
      foundation::Path path = build_directory_ / relative_path;

      switch (type)
      {
      case builder::AssetTypes::kScript:
        ptr = foundation::Memory::Construct<ScriptAsset>(alloc, path);
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
          relative_path.ToString(),
          foundation::Memory::MakeShared(ptr)
        });
    }

    //--------------------------------------------------------------------------
    bool AssetManager::Exists(
      builder::AssetTypes type,
      const foundation::String& path) const
    {
      foundation::String p = NoExtensionToBuildPath(type, path);

      return registered_.find(p) != registered_.end();
    }

    //--------------------------------------------------------------------------
    bool AssetManager::IsLoaded(
      builder::AssetTypes type,
      const foundation::String& path) const
    {
      foundation::String p = NoExtensionToBuildPath(type, path);

      return registered_.at(p)->is_loaded();
    }

    //--------------------------------------------------------------------------
    void AssetManager::set_build_directory(const foundation::Path& path)
    {
      build_directory_ = path;
    }
  }
}