#pragma once

#include <cinttypes>

namespace snuffbox
{
  namespace graphics
  {
    /**
    * @brief Used to retrieve different uniform properties of constant
    *        uniform buffers
    */
    enum class ShaderUniforms
    {
      kPerFrameData, //!< The per-frame data buffer
      kPerObjectData //!< The per-object data buffer
    };

    /**
    * @brief Used to query constants within the shader on the CPU
    *
    * @author Daniel Konings
    */
    class ShaderConstants
    {

    public:

      /**
      * @brief Retrieves the uniform location for a constant uniform buffer
      *
      * @param[in] uniform The uniform to retrieve the bind location for
      *
      * @return The found index
      */
      static uint32_t GetUniformLocation(ShaderUniforms uniform);
    };
  }
}