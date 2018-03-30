#pragma once

#include <EASTL/string.h>
#include "foundation/memory/allocators/eastl_allocator.h"

namespace snuffbox
{
  namespace foundation
  {
    /**
    * @brief An EASTL string with a custom allocator
    */
    using String = eastl::basic_string<char, EASTLAllocator>;
  }
}