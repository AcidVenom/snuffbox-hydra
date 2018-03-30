#pragma once

#include <EASTL/vector.h>
#include "foundation/memory/allocators/eastl_allocator.h"

namespace snuffbox
{
  namespace foundation
  {
    /**
    * @brief An EASTL vector with a custom allocator
    *
    * @tparam T The type contained within the vector
    */
    template <typename T>
    using Vector = eastl::vector<T, EASTLAllocator>;
  }
}