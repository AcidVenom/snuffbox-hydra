#pragma once

#ifndef EASTL_USER_CONFIG_HEADER
#define EASTL_USER_CONFIG_HEADER "foundation/memory/eastl_config.h"
#endif

#include <EASTL/map.h>
#include <EASTL/unordered_map.h>
#include <EASTL/hash_set.h>

namespace snuffbox
{
  namespace foundation
  {
    /**
    * @brief An EASTL map with a custom allocator
    */
    template <typename Key, typename Value>
    using Map = eastl::map<Key, Value, eastl::less<Key>, EASTLAllocator>;

    /**
    * @brief An unordered EASTL map with a custom allocator
    */
    template <typename Key, typename Value>
    using UMap = eastl::hash_map<
      Key, Value, eastl::hash<Key>, eastl::equal_to<Key>, EASTLAllocator>;

    /**
    * @brief An EASTL hash set with a custom allocator
    */
    template <typename Value>
    using HashSet = eastl::hash_set<
      Value, eastl::hash<Value>, eastl::equal_to<Value>, EASTLAllocator>;
  }
}