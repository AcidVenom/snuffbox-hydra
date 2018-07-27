#pragma once

#include "engine/assets/asset.h"

namespace snuffbox
{
  namespace engine
  {
    class RendererService;
    class AssetService;
    class ShaderAsset;

    /**
    * @brief Used to load material assets from raw JSON serialized data
    *
    * @see MaterialCompiler
    *
    * @author Daniel Konings
    */
    class MaterialAsset : public IAsset
    {

    public:

      /**
      * @see IAsset::IAsset
      */
      MaterialAsset(
        const foundation::Path& path, 
        const foundation::String& name);

    protected:

      /**
      * @brief Creates the GPU handle for the material to be loaded in
      *        MaterialAsset::LoadImpl
      */
      void CreateGPUHandle();

      /**
      * @see IAsset::LoadImpl
      */
      bool LoadImpl(const foundation::Path& path) override;

      /**
      * @see IAsset::UnloadImpl
      */
      void UnloadImpl() override;

      /**
      * @brief Releases the created GPU handle
      */
      void Release();

      /**
      * @brief Removes shader dependencies if they are unused
      */
      void RemoveDependencies();

    public:

      /**
      * @return The GPU handle of this material
      */
      void* gpu_handle() const;

      /**
      * @see MaterialAsset::Release
      */
      ~MaterialAsset();

    private:

      ShaderAsset* vs_; //!< The vertex shader of this material
      ShaderAsset* ps_; //!< The pixel shader of this material
      ShaderAsset* gs_; //!< The geometry shader of this material

      void* gpu_handle_; //!< The GPU-created material

      RendererService* renderer_; //!< The renderer service
      AssetService* asset_service_; //!< The asset service
    };
  }
}