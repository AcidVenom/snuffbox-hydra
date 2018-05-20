#include "graphics/ogl/linux/linux_ogl_context.h"
#include <GL/glx.h>

namespace snuffbox
{
  namespace graphics
  {
    //--------------------------------------------------------------------------
    bool LinuxOGLContext::Initialize(void* handle)
    {
      display_ = XOpenDisplay(NULL);

      if (display_ == NULL)
      {
        return false;
      }

      GLint flags[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };

      window_ = reinterpret_cast<Window>(handle);
      XVisualInfo* vi = glXChooseVisual(display_, 0, flags);

      if (vi == NULL)
      {
        return false;
      }

      GLXContext glc = glXCreateContext(display_, vi, NULL, GL_TRUE);
      glXMakeCurrent(display_, window_, glc);

      swap_ = reinterpret_cast<glXSwapInterval>(glXGetProcAddress(
            reinterpret_cast<const GLubyte*>("glXSwapIntervalMESA")));

      if (swap_ == NULL)
      {
        return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------
    void LinuxOGLContext::Swap(bool vsync)
    {
      swap_(vsync == true ? 1 : 0);
      glXSwapBuffers(display_, window_);
    }
  }
}
