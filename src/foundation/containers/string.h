#pragma once

#ifndef EASTL_USER_CONFIG_HEADER
#define EASTL_USER_CONFIG_HEADER "foundation/memory/eastl_config.h"
#endif

#include <EASTL/string.h>

#include <cinttypes>

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