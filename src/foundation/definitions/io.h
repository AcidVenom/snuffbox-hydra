#pragma once

#include "foundation/containers/function.h"
#include "foundation/io/path.h"

#include <cinttypes>

namespace snuffbox
{
  namespace foundation
  {
    /**
    * @brief Used to set file flags for opening a file
    *
    * The flags can be piped together to create combinations for open modes
    */
    enum FileFlags : int32_t
    {
      kRead = 1 << 0, //!< Read only
      kWrite = 1 << 1 //!< Write only
    };

    /**
    * @brief An on directory changed callback for the directory listeners to
    *        use
    *
    * @see Win32DirectoryListener
    * @see LinuxDirectoryListener
    *
    * @param[in] path The path to the file or directory that changed
    */
    using OnDirectoryChanged = Function<void(const Path& path)>;
  }
}