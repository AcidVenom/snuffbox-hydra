#pragma once

#ifndef EASTL_USER_CONFIG_HEADER
#define EASTL_USER_CONFIG_HEADER "foundation/memory/eastl_config.h"
#endif

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