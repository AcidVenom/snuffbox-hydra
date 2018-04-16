#pragma once

#include "foundation/memory/allocators/eastl_allocator.h"

#include <EASTL/queue.h>

namespace snuffbox
{
  namespace foundation
  {
    template <typename T>
    using Queue = eastl::queue<T, eastl::deque<T, EASTLAllocator>>;
  }
}