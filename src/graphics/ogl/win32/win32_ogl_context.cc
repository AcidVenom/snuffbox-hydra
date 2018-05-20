#include "graphics/ogl/win32/win32_ogl_context.h"

#include <foundation/auxiliary/logger.h>

#include <gl/GL.h>
#include <gl/GLU.h>

namespace snuffbox
{
  namespace graphics
  {
    //--------------------------------------------------------------------------
    Win32OGLContext::Win32OGLContext()
    {

    }

    //--------------------------------------------------------------------------
    bool Win32OGLContext::Initialize(void* handle)
    {
      PIXELFORMATDESCRIPTOR pfd =
      {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32,
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24,
        8,
        0,
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
      };

      hdc_ = GetDC(reinterpret_cast<HWND>(handle));

      int pixel_format = ChoosePixelFormat(hdc_, &pfd); 
      SetPixelFormat(hdc_, pixel_format, &pfd);

      hglrc_ = wglCreateContext(hdc_);

      if (hglrc_ == nullptr)
      {
        return false;
      }

      wglMakeCurrent(hdc_, hglrc_);

      foundation::Logger::LogVerbosity<2>(
        foundation::LogChannel::kEngine,
        foundation::LogSeverity::kInfo,
        "OpenGL version: {0}",
        reinterpret_cast<const char*>(glGetString(GL_VERSION)));

      swap_ = reinterpret_cast<wglSwapIntervalEXT>(
        wglGetProcAddress("wglSwapIntervalEXT"));

      if (swap_ == nullptr)
      {
        return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------
    void Win32OGLContext::Swap(bool vsync)
    {
      swap_(vsync == true ? 1 : 0);
      SwapBuffers(hdc_);
    }

    //--------------------------------------------------------------------------
    Win32OGLContext::~Win32OGLContext()
    {
      if (hglrc_ != nullptr)
      {
        wglDeleteContext(hglrc_);
      }
    }
  }
}