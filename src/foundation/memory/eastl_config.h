#pragma once

#include "foundation/memory/allocators/eastl_allocator.h"

namespace snuffbox
{
  namespace foundation
  {
    /**
    * @brief As the EASTL allocator here simply uses the memory interface,
    *        create a static instance of it
    */
    static snuffbox::foundation::EASTLAllocator default_eastl_alloc;
  }
}

#define EA_COMPILER_NO_STRUCTURED_BINDING
#define EASTLAllocatorType snuffbox::foundation::EASTLAllocator
#define EASTLAllocatorDefault() &snuffbox::foundation::default_eastl_alloc;