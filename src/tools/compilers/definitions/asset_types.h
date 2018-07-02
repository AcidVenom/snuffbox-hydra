#pragma once

#include <scripting/script_class.h>
#include <foundation/containers/string.h>

namespace snuffbox
{
  namespace compilers
  {
    /**
    * @brief The different asset types that can be built by the builder and
    *        read by the engine
    *
    * The asset types should be in the order of dependencies. For instance,
    * a material depends on a shader and a texture, so the material enum
    * value should always be later than both of these.
    *
    * @see BuildScheduler
    */
    SCRIPT_ENUM() enum class AssetTypes
    {
      kScript,
      kScene,
      kVertexShader,
      kPixelShader,
      kGeometryShader,
      kMaterial,
      kModel,
      kCount
    };

    /**
    * @brief Converts an asset type to its corresponding build extension
    *
    * @param[in] type The asset type
    *
    * @return The converted extension
    */
    const char* AssetTypesToBuildExtension(AssetTypes type);

    /**
    * @brief Converts a build extension into its original asset type
    *
    * @param[in] ext The extension to convert
    *
    * @return The converted asset type
    */
    AssetTypes AssetTypesFromBuildExtension(const char* ext);

    /**
    * @brief Converts an asset type to its corresponding source extension
    *
    * @param[in] type The asset type
    *
    * @return The converted extension
    */
    const char* AssetTypesToSourceExtension(AssetTypes type);

    /**
    * @brief Converts a source extension into its original asset type
    *
    * @param[in] ext The extension to convert
    *
    * @return The converted asset type
    */
    AssetTypes AssetTypesFromSourceExtension(const char* ext);

    /**
    * @brief Converts a build extension into its source extension
    *
    * @param[in] ext_a The extension to convert 
    * @param[out] ext_b The converted extension
    *
    * @return Was there an extension for this build directory extension?
    */
    bool BuildToSourceExtension(
      const char* ext_a,
      foundation::String* ext_b);

    /**
    * @brief Converts an asset type to its named value
    *
    * @param[in] type The type of the asset to convert
    *
    * @return The converted value
    */
    const char* AssetTypesToString(AssetTypes type);
  }
}

SCRIPT_ENUM_DECL(snuffbox::compilers::AssetTypes);