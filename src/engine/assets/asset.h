#pragma once

#include "engine/application/application.h"
#include "engine/services/asset_service.h"

#include <tools/compilers/definitions/asset_types.h>

#include <scripting/script_class.h>

#include <foundation/io/path.h>
#include <foundation/containers/vector.h>

#include <foundation/serialization/load_archive.h>
#include <foundation/serialization/save_archive.h>

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
    SCRIPT_CLASS() class IAsset : public scripting::ScriptClass
    {

    public:

      SCRIPT_NAME(Asset);

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

    /**
    * @brief A serializable asset handle
    *
    * @author Daniel Konings
    */
    struct SerializableAsset
    {
      compilers::AssetTypes type; //!< The type of the asset
      foundation::String name; //!< The name of the asset
      IAsset* handle; //!< The handle to the asset
    };
  }

  namespace foundation
  {
    //--------------------------------------------------------------------------
    template <>
    inline void SaveArchive::Serialize<engine::SerializableAsset>(
      SaveArchive& archive, 
      const engine::SerializableAsset& asset)
    {
      compilers::AssetTypes type = 
        asset.handle == nullptr ? compilers::AssetTypes::kCount : asset.type;
      
      foundation::String name = 
        asset.handle == nullptr ? "" : asset.name;

      archive(
        SET_ARCHIVE_PROP(type),
        SET_ARCHIVE_PROP(name));
    }

    //--------------------------------------------------------------------------
    template <>
    inline void LoadArchive::Deserialize<engine::SerializableAsset>(
      LoadArchive& archive, 
      engine::SerializableAsset* out)
    {
      compilers::AssetTypes type;
      foundation::String name;

      archive(
        GET_ARCHIVE_PROP(type),
        GET_ARCHIVE_PROP(name));

      out->type = type;
      out->name = name;

      if (out->type == compilers::AssetTypes::kCount)
      {
        out->handle = nullptr;
        return;
      }

      engine::AssetService* as = 
        engine::Application::Instance()->GetService<engine::AssetService>();

      out->handle = as->Get(type, name);
    }
  }
}