#pragma once

#include "graphics/renderer_loader.h"

namespace snuffbox
{
  namespace graphics
  {
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
      * @see IRendererLoader::LoadShader
      */
      GPUHandle LoadShader(
        ShaderTypes type,
        const uint8_t* buffer,
        size_t len) override;

      /**
      * @see IRendererLoader::ReleaseShader
      */
      void ReleaseShader(GPUHandle shader) override;
    };
  }
}