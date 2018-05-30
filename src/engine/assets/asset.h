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
      * @return Was the load a success?
      */
      bool Load();

      /**
      * @return The type of this asset
      */
      builder::AssetTypes type() const;

    protected:

      /**
      * @brief Loads the asset from a buffer of raw data
      *
      * @param[in] path The path to the file to load
      *
      * @return Were we able to load the asset?
      */
      virtual bool LoadImpl(const foundation::Path& path) = 0;

    private:

      builder::AssetTypes type_; //!< The type of this asset
      foundation::Path path_; //!< The path to the file to load
    };
  }
}