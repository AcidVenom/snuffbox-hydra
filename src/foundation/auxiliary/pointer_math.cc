#include "foundation/auxiliary/pointer_math.h"

namespace snuffbox
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    size_t PointerMath::AlignDelta(void* ptr, size_t align)
    {
      size_t delta = align - 
        ((reinterpret_cast<size_t>(ptr)) & 
        (static_cast<size_t>(align - 1)));

      if (delta == align)
      {
        delta = 0;
      }

      return delta;
    }
  }
}