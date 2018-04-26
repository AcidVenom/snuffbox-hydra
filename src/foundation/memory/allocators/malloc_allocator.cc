#include "foundation/memory/allocators/malloc_allocator.h"
#include "foundation/auxiliary/pointer_math.h"

#include <memory>
#include <cstring>

namespace snuffbox
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    MallocAllocator::MallocAllocator(size_t max_size) :
      Allocator(max_size)
    {

    }

    //--------------------------------------------------------------------------
    void* MallocAllocator::AllocateImpl(size_t size, size_t /*align*/)
    {
      size_t header_size = sizeof(AllocationHeader);

      size_t total_size = size + header_size;
      void* base_addr = malloc(total_size);

      AllocationHeader header;
      header.size = size;

      memcpy(base_addr, &header, header_size);

      return PointerMath::Offset(base_addr, header_size);
    }

    //--------------------------------------------------------------------------
    size_t MallocAllocator::DeallocateImpl(void* ptr)
    {
      intptr_t header_size = static_cast<intptr_t>(sizeof(AllocationHeader));

      AllocationHeader* header = reinterpret_cast<AllocationHeader*>(
        PointerMath::Offset(ptr, -header_size));

      size_t size = header->size;

      free(header);

      return size;
    }

    //--------------------------------------------------------------------------
    size_t MallocAllocator::GetSize(void* ptr)
    {
      if (ptr == nullptr)
      {
        return 0;
      }

      intptr_t header_size = static_cast<intptr_t>(sizeof(AllocationHeader));

      AllocationHeader* header = reinterpret_cast<AllocationHeader*>(
        PointerMath::Offset(ptr, -header_size));

      size_t size = header->size;

      return size;
    }
  }
}
