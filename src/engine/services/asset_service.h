#pragma once

#include "engine/services/service.h"

#include <tools/compilers/definitions/asset_types.h>

#include <foundation/io/path.h>
#include <foundation/containers/map.h>
#include <foundation/memory/memory.h>

namespace snuffbox
{
  namespace engine
  {
    class IAsset;
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
      * @brief Used to store asset information of an asset in the build
      *        directory
      *
      * @author Daniel Konings
      */
      struct AssetFile
      {
        foundation::Path relative_path; //!< The relative path to the file
        compilers::AssetTypes type; //!< The asset type of the file
        bool is_directory; //!< Is the path a directory?
      };

      /**
      * @brief Enumerates all files in a specified directory and returns
      *        the valid asset paths
      *
      * @param[in] dir The directory to enumerate the assets from
      * @param[in] root The root directory where the enumeration started
      * @param[in] recursive Should the assets be enumerated recursively?
      * @param[in] include_directories Should directories be included?
      *
      * @return A list of asset file definitions with both type and path
      */
      static foundation::Vector<AssetFile> EnumerateAssets(
        const foundation::Path& dir,
        const foundation::Path& root,
        bool recursive = false,
        bool include_directories = false);

      /**
      * @brief Converts a build path without an extension to a build path with
      *        the respective extension of that asset type
      *
      * @param[in] type The type of the asset
      * @param[in] path The path to the built file
      */
      static foundation::String NoExtensionToBuildPath(
        compilers::AssetTypes type,
        const foundation::String& path);

      /**
      * @see IService::IService
      */
      AssetService();

    protected:

      /**
      * @brief Checks if a path is not a directory and a valid asset type
      *
      * @param[in] path The path
      *
      * @return Is the path an asset?
      */
      static bool IsAsset(const foundation::Path& path);

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
      * @brief Refreshes the underlying asset service by unloading all assets
      *        and registering new ones from a provided path
      *
      * @see AssetService::Clear
      *
      * @param[in] path The path to the build directory
      */
      void Refresh(const foundation::Path& path);

      /**
      * @brief Clears every asset in the asset service and unloads them
      */
      void Clear();

      /**
      * @brief Registers an asset from a provided path
      *
      * When an asset has been registered, it can be loaded and unloaded
      * when required. The data within the asset is the only thing that changes,
      * whereas the handle to the asset is always valid during execution.
      *
      * @param[in] relative_path The relative path to the asset to register
      */
      void RegisterAsset(const foundation::Path& relative_path);

      /**
      * @see AssetService::RegisterAsset
      *
      * @remarks When we already know the type of an asset, we can register
      *          it immediately
      *
      * This method will fail if the asset type is AssetTypes::kUnknown or
      * AssetTypes::kCount
      *
      * @param[in] type The type of the asset
      */
      void RegisterAsset(
        compilers::AssetTypes type, 
        const foundation::Path& relative_path);

      /**
      * @brief Removes an asset from the list of registered assets and unloads
      *        it if it was loaded
      *
      * @param[in] type The type of the asset
      * @param[in] relative_path The relative path to the asset to remove
      */
      void RemoveAsset(
        compilers::AssetTypes type,
        const foundation::Path& relative_path);

      /**
      * @brief Checks if an asset exists
      *
      * @param[in] type The type of the asset to check for
      * @param[in] path The path to the asset, without an extension
      *
      * @return Does the asset exist?
      */
      bool Exists(
        compilers::AssetTypes type, 
        const foundation::String& path) const;

      /**
      * @brief Loads an asset by type and relative path with no extension
      *
      * @param[in] type The type of the registered asset to load
      * @param[in] path The relative path to the asset
      *
      * @return Were we able to load the asset?
      */
      bool Load(
        compilers::AssetTypes type, 
        const foundation::String& path);
      
      /**
      * @brief Reloads an asset if it was already loaded
      *
      * @see AssetService::Load
      */
      bool Reload(
        compilers::AssetTypes type,
        const foundation::String& path);

      /**
      * @brief Unlaods an asset by type and relative path with no extension
      *
      * @param[in] type The type of the registered asset to unload
      * @param[in] path The relative path to the asset
      */
      void Unload(
        compilers::AssetTypes type,
        const foundation::String& path);

      /**
      * @brief Loads all assets of a specific type
      *
      * @param[in] type The type of the assets to load
      *
      * @return Were all assets loaded succesfully?
      */
      bool LoadAll(compilers::AssetTypes type);

      /**
      * @brief Unloads all assets of a specific type
      *
      * @param[in] type The type of the assets to unload
      */
      void UnloadAll(compilers::AssetTypes type);

      /**
      * @brief Checks if an asset has already been loaded
      *
      * @param[in] type The type of the asset to check
      * @param[in] path The path to the file without an extension
      *
      * @return Was the asset already loaded?
      */
      bool IsLoaded(
        compilers::AssetTypes type, 
        const foundation::String& path) const;

      /**
      * @brief Retrieves an asset by type and path
      *
      * @param[in] type The type of the asset to retrieve
      * @param[in] path The path to the asset, with no extension
      *
      * @return The found asset, or nullptr if it doesn't exist
      */
      IAsset* Get(compilers::AssetTypes type, const foundation::String& path);

      /**
      * @brief Sets the build directory of the asset service, to append
      *        to the relative file paths
      *
      * @param[in] path The path to the build directory
      */
      void set_build_directory(const foundation::Path& path);

    private:

      using AssetMap =
        foundation::UMap<foundation::String, foundation::SharedPtr<IAsset>>;

      /**
      * @brief The asset map for all known asset types that can be loaded
      *        and unloaded
      */
      AssetMap registered_;
      foundation::Path build_directory_; //!< The current build directory
    };
  }
}