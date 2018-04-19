#pragma once

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
  }
}