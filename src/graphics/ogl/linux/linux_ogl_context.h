#pragma once

#include <X11/Xlib.h>

namespace snuffbox
{
  namespace graphics
  {
    /**
    * @brief Used to create a Linux OpenGL context using
    *        GLX and its extensions
    *
    * @author Daniel Konings
    */
    class LinuxOGLContext
    {

    public:

      /**
      * @brief Initializes the context
      *
      * @param[in] handle The native X11 window handle
      *
      * @return Was initialization a success?
      */
      bool Initialize(void* handle);

      /**
      * @brief Swaps the GLX buffers
      *
      * @param[in] vsync Should we wait for vertical sync?
      */
      void Swap(bool vsync);

    private:

      /**
      * @brief The type of the GLX swap function
      */
      using glXSwapInterval = int(*)(int);

      Display* display_; //!< The current X11 display
      Window window_; //!< The current X11 window
      glXSwapInterval swap_; //!< The GLX swap function pointer
    };
  }
}
