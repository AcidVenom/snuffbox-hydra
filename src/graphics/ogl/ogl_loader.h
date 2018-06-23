#pragma once

#include "graphics/renderer_loader.h"

namespace snuffbox
{
  namespace graphics
  {
    class OGLShader;

    /**
    * @brief Used to load OpenGL specific GPU resources
    *
    * @see IRendererLoader
    *
    * @author Daniel Konings
    */
    class OGLLoader : public IRendererLoader
    {

    public:

      /**
      * @see IRendererLoader::CreateShader
      */
      GPUHandle CreateShader(ShaderTypes type) override;

      /**
      * @see IRendererLoader::LoadShader
      */
      bool LoadShader(
        GPUHandle handle,
        const uint8_t* buffer,
        size_t len) override;

      /**
      * @see IRendererLoader::UnloadShader
      */
      void UnloadShader(GPUHandle handle) override;

      /**
      * @see IRendererLoader::ReleaseShader
      */
      void ReleaseShader(GPUHandle handle) override;

      /**
      * @see IRendererLoader::CreateMaterial
      */
      GPUHandle CreateMaterial() override;

      /**
      * @see IRendererLoader::LoadMaterial
      */
      bool LoadMaterial(
        GPUHandle handle,
        GPUHandle vs,
        GPUHandle ps,
        GPUHandle gs) override;

      /**
      * @see IRendererLoader::UnloadMaterial
      */
      void UnloadMaterial(GPUHandle handle) override;

      /**
      * @see IRendererLoader::ReleaseMaterial
      */
      void ReleaseMaterial(GPUHandle handle) override;
    };
  }
}