#pragma once

#include "graphics/definitions/texture_formats.h"

namespace snuffbox
{
  namespace graphics
  {
    /**
    * @brief Used to construct render targets and hold the native renderer
    *        handle of the constructed target
    *
    * @author Daniel Konings
    */
    struct RenderTarget
    {
      TextureFormats format; //!< The format of this render target
      void* native_handle; //!< The native handle within the renderers

      /**
      * @brief The maximum render targets that can be bound to a shader
      */
      static const int kMaxBoundTargets = 8;
    };
  }
}