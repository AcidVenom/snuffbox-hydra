#pragma once

#include "foundation/memory/allocators/eastl_allocator.h"

#include <EASTL/queue.h>
#include <EASTL/priority_queue.h>

namespace snuffbox
{
  namespace foundation
  {
    /**
    * @brief A regular EASTL deque
    */
    template <typename T>
    using Queue = eastl::queue<T, eastl::deque<T, EASTLAllocator>>;

    /**
    * @brief An EASTL priority queue
    */
    template <typename T, typename Y = eastl::less<T>>
    using PriorityQueue = 
      eastl::priority_queue<T, eastl::vector<T, EASTLAllocator>, Y>;
  }
}