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
      * @brief Used to create the shader handles within the native rendering API
      *
      * @param[in] type The type of the shader to load
      *
      * @retunr The created GPU handle
      */
      virtual GPUHandle CreateShader(ShaderTypes type) = 0;

      /**
      * @brief Used to actually load a created shader
      *
      * @see IRendererLoader::CreateShader
      *
      * @param[in] buffer The buffer to read from
      * @param[in] len The length of the buffer
      *
      * @return Were we able to load the shader?
      */
      virtual bool LoadShader(
        GPUHandle handle, 
        const uint8_t* data, 
        size_t len) = 0;

      /**
      * @brief Unloads a loaded shader
      *
      * @param[in] handle The shader to unload
      */
      virtual void UnloadShader(GPUHandle handle) = 0;

      /**
      * @brief Releases a created shader
      *
      * @param[in] shader The shader to release
      */
      virtual void ReleaseShader(GPUHandle shader) = 0;
    };
  }
}