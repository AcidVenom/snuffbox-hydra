#pragma once

#include "graphics/definitions/frame_data.h"
#include "graphics/renderer_loader.h"

namespace snuffbox
{
  namespace graphics
  {
    /**
    * @brief Used to queue draw commands within the renderers
    *
    * @author Daniel Konings
    */
    struct DrawCommand
    {
      PerObjectData data; //!< The per-object data
      IRendererLoader::GPUHandle mesh; //!< The mesh to render
      IRendererLoader::GPUHandle material; //!< The material to render with
    };
  }
}