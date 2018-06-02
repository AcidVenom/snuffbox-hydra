#pragma once

#include <tools/builder/definitions/asset_types.h>

#include <foundation/io/path.h>
#include <foundation/containers/map.h>
#include <foundation/memory/memory.h>

namespace snuffbox
{
  namespace engine
  {
    class IAsset;

    /**
    * @brief The asset manager to load and unload assets with and that keeps
    *        references to loaded assets in a map structure
    *
    * The assets can be retrieved by their relative path in the build directory.
    * This applies for both the editor and the engine itself.
    *
    * @author Daniel Konings
    */
    class AssetManager
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
        builder::AssetTypes type; //!< The asset type of the file
      };

      /**
      * @brief Enumerates all files in a specified directory and returns
      *        the valid asset paths
      *
      * @param[in] dir The directory to enumerate the assets from
      * @param[in] recursive Should the assets be enumerated recursively?
      *
      * @return A list of asset file definitions with both type and path
      */
      static foundation::Vector<AssetFile> EnumerateAssets(
        const foundation::Path& dir, 
        bool recurisve = false);

      /**
      * @brief Clears every asset in the asset manager and unloads them
      */
      void Clear();

      /**
      * @brief Loads an asset by type and relative path with no extension
      *
      * @param[in] type The type of the registered asset to load
      * @param[in] path The relative path to the asset
      *
      * @return Were we able to load the asset?
      */
      bool Load(builder::AssetTypes type, const foundation::String& path);

      /**
      * @brief Loads all assets of a specific type
      *
      * @param[in] type The type of the assets to load
      */
      void LoadAll(builder::AssetTypes type);

    protected:

      /**
      * @brief Checks if a path is not a directory and a valid asset type
      *
      * @param[in] path The path
      *
      * @return Is the path an asset?
      */
      static bool IsAsset(const foundation::Path& path);

    public:

      /**
      * @brief Converts a build path without an extension to a build path with
      *        the respective extension of that asset type
      *
      * @param[in] type The type of the asset
      * @param[in] path The path to the built file
      */
      static foundation::String NoExtensionToBuildPath(
        builder::AssetTypes type,
        const foundation::String& path);

      /**
      * @brief Registers an asset from a provided path
      *
      * When an asset has been registered, it can be loaded and unloaded
      * when required. The data within the asset is the only thing that changes,
      * whereas the handle to the asset is always valid during execution.
      *
      * @remarks If the asset is already registered and it is already loaded,
      *          it is reloaded
      *
      * @param[in] relative_path The relative path to the asset to register
      */
      void RegisterAsset(const foundation::Path& asset);

      /**
      * @see AssetManager::RegisterAsset
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
        builder::AssetTypes type, 
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
        builder::AssetTypes type, 
        const foundation::String& path) const;

      /**
      * @brief Checks if an asset has already been loaded
      *
      * @param[in] type The type of the asset to check
      * @param[in] path The path to the file without an extension
      *
      * @return Was the asset already loaded?
      */
      bool IsLoaded(
        builder::AssetTypes type,
        const foundation::String& path) const;

      /**
      * @brief Sets the build directory of the asset manager, to append
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