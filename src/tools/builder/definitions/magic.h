#pragma once

#include <cinttypes>

namespace snuffbox
{
  namespace builder
  {
    /**
    * @brief The magic numbers to put in the header of a compiled file
    *        to make sure the file is of the correct type during decompilation
    */
    enum class FileHeaderMagic : int32_t
    {
      kScript = 0x52435373 //!< "sSCR" As a hexadecimal value
    };
  }
}