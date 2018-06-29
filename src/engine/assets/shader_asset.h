#pragma once

#include "engine/assets/asset.h"

namespace snuffbox
{
  namespace engine
  {
    class RendererService;
    class MaterialAsset;

    /**
    * @brief Used to load vertex, pixel and geometry shaders from
    *        SPIRV-Cross generated bytecode
    *
    * @see ShaderCompiler
    *
    * @author Daniel Konings
    */
    class ShaderAsset : public IAsset
    {

      friend MaterialAsset;

    public:

      /**
      * @see IAsset::IAsset
      *
      * @param[in] type The shader type, valid arguments are:
      *                 - AssetTypes::kVertexShader
      *                 - AssetTypes::kPixelShader
      *                 - AssetTypes::kGeometryShader
      */
      ShaderAsset(
        const foundation::Path& path,
        compilers::AssetTypes type);

    protected:

      /**
      * @brief Creates the GPU handle to do all loading operations on
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
      * @return The GPU handle of this shader
      */
      void* gpu_handle() const;

    public:

      /**
      * @brief Releases the underlying GPU handle
      */
      ~ShaderAsset();

    private:

      void* gpu_handle_; //!< The GPU handle for the created shader
      RendererService* renderer_; //!< The renderer service
    };
  }
}