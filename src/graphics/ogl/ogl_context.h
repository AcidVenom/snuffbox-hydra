#pragma once

#ifdef SNUFF_WIN32
#include "graphics/ogl/win32/win32_ogl_context.h"
namespace snuffbox
{
  namespace graphics
  {
    using OGLContext = Win32OGLContext;
  }
}
#elif defined (SNUFF_LINUX)
#include "graphics/ogl/linux/linux_ogl_context.h"
namespace snuffbox
{
  namespace graphics
  {
    using OGLContext = LinuxOGLContext;
  }
}
#else
#error "Unsupported compiler platform"
#endif
