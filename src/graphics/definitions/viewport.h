#pragma once

namespace snuffbox
{
  namespace graphics
  {
    /**
    * @brief Used to set viewports in the native rendering APIs
    *
    * @author Daniel Konings
    */
    struct Viewport
    {
      float x; //!< The x location of the viewport
      float y; //!< The y location of the viewport
      float width; //!< The width of the viewport
      float height; //!< The height of the viewport
    };
  }
}