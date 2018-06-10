#include "foundation/memory/allocators/rapidjson_allocator.h"

#include "foundation/memory/memory.h"

namespace snuffbox
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    const bool RapidJsonStackAllocator::kNeedFree = true;
    const size_t RapidJsonStackAllocator::kMaxSize_ = 1024ul * 1024ul * 4ul;

    //--------------------------------------------------------------------------
    RapidJsonStackAllocator::RapidJsonStackAllocator() :
      MallocAllocator(kMaxSize_)
    {

    }

    //--------------------------------------------------------------------------
    void* RapidJsonStackAllocator::Malloc(size_t size)
    {
      return Memory::Allocate(size, this);
    }

    //--------------------------------------------------------------------------
    void* RapidJsonStackAllocator::Realloc(
      void* original,
      size_t original_size,
      size_t new_size)
    {
      size_t old_size = GetSize(original);
      void* block = Memory::Allocate(new_size, this);

      if (new_size >= old_size)
      {
        memcpy(block, original, old_size);
      }
      else
      {
        memcpy(block, original, new_size);
      }

      Memory::Deallocate(original);

      return block;
    }

    //--------------------------------------------------------------------------
    void RapidJsonStackAllocator::Free(void* ptr)
    {
      Memory::Deallocate(ptr);
    }
  }
}