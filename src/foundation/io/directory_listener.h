#pragma once

#ifdef SNUFF_WIN32
#include "foundation/win32/win32_directory_listener.h"
namespace snuffbox
{
  namespace foundation
  {
    using DirectoryListener = Win32DirectoryListener;
  }
}
#elif defined (SNUFF_LINUX)
#include "foundation/linux/linux_directory_listener.h"
namespace snuffbox
{
  namespace foundation
  {
    using DirectoryListener = LinuxDirectoryListener;
  }
}
#else
#error "Unsupported compiler platform"
#endif