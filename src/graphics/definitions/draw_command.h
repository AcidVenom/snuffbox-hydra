#pragma once

#include "graphics/definitions/frame_data.h"

namespace snuffbox
{
  namespace graphics
  {
    /**
    * @brief Used to queue draw commands within the renderers
    *
    * @author Daniel Konings
    */
    class DrawCommand
    {
      PerObjectData data; //!< The per-object data
    };
  }
}