#pragma once

#include "foundation/memory/allocators/eastl_allocator.h"
#include <EASTL/functional.h>

namespace snuffbox
{
  namespace foundation
  {
    /**
    * @brief An EASTL function
    */
    template <typename T>
    using Function = eastl::function<T>;
  }
}
