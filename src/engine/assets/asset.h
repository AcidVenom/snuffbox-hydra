#pragma once

#include <tools/compilers/definitions/asset_types.h>

#include <foundation/io/path.h>
#include <foundation/containers/vector.h>

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
      IAsset(compilers::AssetTypes type, const foundation::Path& path);

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
      compilers::AssetTypes type() const;

      /**
      * @return The path to the file to load
      */
      const foundation::Path& path() const;

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

      /**
      * @brief Adds a dependent asset to this asset
      *
      * @remarks We first check if the dependency was not added yet,
      *          to avoid duplicate dependencies
      *
      * @param[in] asset The dependency to add
      */
      void AddDependency(IAsset* asset);

      /**
      * @brief Removes a dependency from this asset
      *
      * @param[in] asset The dependency to remove
      */
      void RemoveDependency(IAsset* asset);

    public:

      /**
      * @brief Virtual destructor
      */
      virtual ~IAsset();

    private:

      compilers::AssetTypes type_; //!< The type of this asset
      foundation::Path path_; //!< The path to the file to load

      bool is_loaded_; //!< Has this asset been loaded?

      /**
      * @brief A list of dependencies to load when
      *        this asset has been loaded
      *
      * @remarks When a dependency is unloaded, so is the asset that
      *          depends on it
      */
      foundation::Vector<IAsset*> dependencies_;
    };
  }
}