#pragma once

#include <scripting/script_class.h>

namespace snuffbox
{
  namespace engine
  {
    class IAsset;
    class MaterialAsset;

    /**
    * @brief Used to wrap renderer-specific materials and their GPU handles
    *
    * Materials are mostly used as an asset and thus they'll be used
    * as a shared material. However, whenever a uniform is set on the material,
    * the material will become an instance of the original material.
    *
    * The references to the shaders and underlying GPU material will remain,
    * but the uniform data is instanced so that they are unique per object.
    *
    * @author Daniel Konings
    */
    SCRIPT_CLASS() class Material : public scripting::ScriptClass
    {

    public:

      SCRIPT_NAME(Material);
      SCRIPT_CONSTRUCTOR(Material);

      /**
      * @brief Default constructor
      */
      Material();

      /**
      * @see Material::SetMaterialAsset
      */
      Material(IAsset* asset);

      /**
      * @see Material::SetMaterialAsset
      */
      Material(const foundation::String& name);

      /**
      * @return The wrapped material asset
      */
      MaterialAsset* asset() const;

    protected:

      /**
      * @brief Set a material asset directly
      *
      * @param[in] asset The material asset to set
      *
      * @remarks The material asset is checked if it's an actual material,
      *          else it will not be set
      */
      void SetMaterialAsset(IAsset* asset);

      /**
      * @brief Set a material by a material asset's name
      *
      * @param[in] name The name of the material asset
      */
      void SetMaterialAsset(const foundation::String& name);

    private:

      MaterialAsset* asset_; //!< The wrapped material asset
    };
  }
}