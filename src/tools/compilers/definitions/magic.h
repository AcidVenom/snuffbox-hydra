#pragma once

#include <cinttypes>

namespace snuffbox
{
  namespace compilers
  {
    /**
    * @brief The magic numbers to put in the header of a compiled file
    *        to make sure the file is of the correct type during decompilation
    */
    enum class FileHeaderMagic : int32_t
    {
      kScript = 0x52435373, //!< "sSCR" As a hexadecimal value
      kScene = 0x45435373, //!< "sSCE" As a hexadecimal value
      kVertexShader = 0x48535673, //!< "sVSH" As a hexadecimal value
      kPixelShader = 0x48535073, //!< "sPSH" As a hexadecimal value
      kGeometryShader = 0x48534773, //!< "sGSH" As a hexadecimal value
      kMaterial = 0x5441D473, //!< "sMAT" As a hexadecimal value
      kUnknown = 0
    };
  }
}