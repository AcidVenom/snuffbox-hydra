#pragma once

#include <foundation/memory/allocators/eastl_allocator.h>

#include <EASTL/map.h>

namespace snuffbox
{
  namespace foundation
  {
    /**
    * @brief An EASTL map with a custom allocator
    */
    template <typename Key, typename Value>
    using Map = eastl::map<Key, Value, eastl::less<Key>, EASTLAllocator>;
  }
}