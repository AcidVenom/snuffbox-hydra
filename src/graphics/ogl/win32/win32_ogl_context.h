#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#ifdef CreateService
#undef CreateService
#endif

namespace snuffbox
{
  namespace graphics
  {
    /**
    * @brief Used to create an OpenGL context from a HWND handle, retrieved
    *        from either the player or editor application
    *
    * @author Daniel Konings
    */
    class Win32OGLContext
    {

    public:

      /**
      * @brief Creates an invalid context
      */
      Win32OGLContext();

      /**
      * @brief Initializes the context from a window handle
      *
      * @param[in] handle The handle to the window
      */
      bool Initialize(void* handle);

      /**
      * @brief Swaps the OpenGL buffers
      *
      * @param[in] vsync Should we wait for vertical sync?
      */
      void Swap(bool vsync);

      /**
      * @brief Frees the created WGL context
      */
      ~Win32OGLContext();

    private:

      using wglSwapIntervalEXT = BOOL(*)(int);

      HDC hdc_; //!< The device context handle
      HGLRC hglrc_; //!< The OpenGL rendering context
      wglSwapIntervalEXT swap_; //!< The WGL swap extension method
    };
  }
}