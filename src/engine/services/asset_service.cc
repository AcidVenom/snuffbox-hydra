#include "engine/services/asset_service.h"

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
      
    }

    //--------------------------------------------------------------------------
    void AssetService::Refresh(const foundation::Path& path)
    {
      manager_.Clear();
      manager_.set_build_directory(path);

      foundation::Vector<AssetManager::AssetFile> files = 
        AssetManager::EnumerateAssets(path, true);

      for (size_t i = 0; i < files.size(); ++i)
      {
        const AssetManager::AssetFile& file = files.at(i);
        manager_.RegisterAsset(file.type, file.relative_path);
      }
    }

    //--------------------------------------------------------------------------
    void AssetService::RegisterAsset(
      builder::AssetTypes type,
      const foundation::Path& path)
    {
      manager_.RegisterAsset(type, path);
    }

    //--------------------------------------------------------------------------
    bool AssetService::Load(
      builder::AssetTypes type, 
      const foundation::String& path)
    {
      return manager_.Load(type, path);
    }

    //--------------------------------------------------------------------------
    void AssetService::LoadAll(builder::AssetTypes type)
    {
      manager_.LoadAll(type);
    }
  }
}