#pragma once

#include "engine/services/service.h"
#include "engine/assets/asset_manager.h"

namespace snuffbox
{
  namespace engine
  {
    /**
    * @brief Used to manage loading and unloading of assets when required
    *        by the gameplay execution
    *
    * @author Daniel Konings
    */
    class AssetService : public ServiceBase<AssetService>
    {

    public:

      /**
      * @see IService::IService
      */
      AssetService();

    protected:

      /**
      * @see IService::OnInitialize
      */
      foundation::ErrorCodes OnInitialize(Application& app) override;

      /**
      * @see IService::OnShutdown
      */
      void OnShutdown(Application& app) override;

    public:

      /**
      * @brief Refreshes the underlying asset manager by unloading all assets
      *        and registering new ones from a provided path
      *
      * @see AssetManager::Clear
      *
      * @param[in] path The path to the build directory
      */
      void Refresh(const foundation::Path& path);

      /**
      * @see AssetManager::RegisterAsset
      */
      void RegisterAsset(
        builder::AssetTypes type, 
        const foundation::Path& path);

      /**
      * @see AssetManager::Load
      */
      bool Load(builder::AssetTypes type, const foundation::String& path);

      /**
      * @see AssetManager::LoadAll
      */
      bool LoadAll(builder::AssetTypes type);

      /**
      * @see AssetManager::IsLoaded
      */
      bool IsLoaded(builder::AssetTypes type, const foundation::String& path);

    private:

      AssetManager manager_; //!< The asset manager
    };
  }
}