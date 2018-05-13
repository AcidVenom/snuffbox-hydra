#pragma once

#include <foundation/containers/function.h>

#include <cinttypes>

namespace snuffbox
{
  namespace graphics
  {
    /**
    * @brief Used to store a window's information within the renderers
    *
    * @author Daniel Konings
    */
    struct GraphicsWindow
    {
      /**
      * @brief Used to set a callback when a resize happens
      *
      * @param[in] width The new width of the window
      * @param[in] height The new height of the window
      */
      using SizeCallback = 
        foundation::Function<void(uint16_t width, uint16_t height)>;

      void* handle; //!< The window handle
      uint16_t width; //!< The current width of the window in pixels
      uint16_t height; //!< The current height of the window in pixels
    };
  }
}