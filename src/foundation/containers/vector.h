#pragma once

#ifndef EASTL_USER_CONFIG_HEADER
#define EASTL_USER_CONFIG_HEADER "foundation/memory/eastl_config.h"
#endif

#include <EASTL/vector.h>

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