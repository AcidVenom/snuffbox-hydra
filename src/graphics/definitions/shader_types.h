#pragma once

namespace snuffbox
{
  namespace graphics
  {
    /**
    * @brief The different shader types that exist
    *
    * @author Daniel Konings
    */
    enum class ShaderTypes
    {
      kVertex, //!< A vertex shader
      kPixel, //!< A pixel shader
      kGeometry, //!< A geometry shader
      kUnknown = -1
    };
  }
}