#pragma once

#ifdef SNUFF_WIN32
#include "foundation/win32/win32_directory.h"
namespace snuffbox
{
  namespace foundation
  {
    using Directory = Win32Directory;
  }
}
#elif defined (SNUFF_LINUX)
#include "foundation/win32/linux_directory.h"
namespace snuffbox
{
  namespace foundation
  {
    using Directory = LinuxDirectory;
  }
}
#else
#error "Unknown compiler platform"
#endif