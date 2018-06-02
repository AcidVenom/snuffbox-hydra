#pragma once

#include <tools/builder/definitions/asset_types.h>
#include <foundation/io/path.h>

#include <cinttypes>

namespace snuffbox
{
  namespace engine
  {
    /**
    * @brief The base interface for every asset to use, storing type information
    *        and methods to load/reload the asset
    *
    * @author Daniel Konings
    */
    class IAsset
    {

    public:

      /**
      * @brief Construct through an asset type
      *
      * @param[in] type The type of the asset to create
      * @param[in] path The path to the file to load
      *
      * @see IAsset::Load
      */
      IAsset(builder::AssetTypes type, const foundation::Path& path);

      /**
      * @brief Loads a file as an asset
      *
      * @remarks If the extension of the file doesn't match the expected
      *          extension, this function fails
      *
      * This function calls IAsset::LoadImpl, which the derived asset implements
      *
      * If the asset is already loaded, it is unloaded and loaded again to
      * reload it.
      *
      * @return Was the load a success?
      */
      bool Load();

      /**
      * @brief Unloads the asset if it has been loaded
      *
      * This function calls IAsset::UnloadImpl, which the derived asset 
      * implements
      */
      void Unload();

      /**
      * @return The type of this asset
      */
      builder::AssetTypes type() const;

      /**
      * @return Has this asset been loaded?
      */
      bool is_loaded() const;

    protected:

      /**
      * @brief Loads the asset from a path
      *
      * @param[in] path The path to the file to load
      *
      * @return Were we able to load the asset?
      */
      virtual bool LoadImpl(const foundation::Path& path) = 0;

      /**
      * @brief Unloads an already loaded asset
      *
      * @remarks This method does not have any parameters, as any data should
      *          be managed by the derived asset itself
      */
      virtual void UnloadImpl() = 0;

    private:

      builder::AssetTypes type_; //!< The type of this asset
      foundation::Path path_; //!< The path to the file to load

      bool is_loaded_; //!< Has this asset been loaded?
    };
  }
}