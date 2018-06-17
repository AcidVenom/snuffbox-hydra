#pragma once

#include "graphics/definitions/shader_types.h"

#include <cinttypes>
#include <cstddef>

namespace snuffbox
{
  namespace graphics
  {
    /**
    * @brief Used as an interface to load GPU memory from within the
    *        native rendering API (shaders, textures, render targets, etc.)
    *
    * @author Daniel Konings
    */
    class IRendererLoader
    {

    public:

      /**
      * @brief A more descriptive name for the GPU handles
      */
      using GPUHandle = void*;

      /**
      * @brief Used to load shaders within the native rendering API
      *
      * @param[in] type The type of the shader to load
      * @param[in] buffer The buffer to read from
      * @param[in] len The length of the buffer
      *
      * @retunr The created GPU handle, or nullptr if failed
      */
      virtual GPUHandle LoadShader(
        ShaderTypes type, 
        const uint8_t* buffer, 
        size_t len) = 0;

      /**
      * @brief Releases a created shader
      *
      * @param[in] shader The shader to release
      */
      virtual void ReleaseShader(GPUHandle shader) = 0;
    };
  }
}