#pragma once

#ifndef EASTL_FUNCTION_DEFAULT_ALLOCATOR
#define EASTL_FUNCTION_DEFAULT_ALLOCATOR snuffbox::foundation::EASTLAllocator
#endif

#include "foundation/memory/allocators/eastl_allocator.h"
#include <EASTL/functional.h>

namespace snuffbox
{
  namespace foundation
  {
    /**
    * @brief An EASTL function
    *
    * @remarks Seemingly the allocator here cannot be specified in the
    *          type declaration, but is specified through the
    *          EASTL_FUNCTION_DEFAULT_ALLOCATOR macro
    */
    template <typename T>
    using Function = eastl::function<T>;
  }
}