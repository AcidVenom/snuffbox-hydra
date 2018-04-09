#include "foundation/memory/memory.h"
#include "foundation/auxiliary/pointer_math.h"

#include <assert.h>

namespace snuffbox
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    const size_t Memory::kDefaultHeapSize_ = 1024ul * 1024ul * 2ul;
    const size_t Memory::kDefaultAlignment_ = 16ul;

    Memory::DefaultAllocator Memory::default_allocator_(kDefaultHeapSize_);

    //--------------------------------------------------------------------------
    void* Memory::Allocate(size_t size, size_t align, Allocator* allocator)
    {
      if (allocator == nullptr)
      {
        assert(false && "Attempted to allocate memory with a null allocator");
        allocator = &default_allocator_;
      }

      size_t header_size = sizeof(AllocationHeader);
      void* base = reinterpret_cast<AllocationHeader*>(
        allocator->Allocate(size + header_size + align - 1, align));

      void* ptr = PointerMath::Offset(base, header_size);

      size_t a = PointerMath::AlignDelta(ptr, align);
      ptr = PointerMath::Offset(ptr, a);

      AllocationHeader* header = reinterpret_cast<AllocationHeader*>(
        PointerMath::Offset(base, a));

      header->allocator = allocator;
      header->size = size;
      header->align = a;

      return ptr;
    }

    //--------------------------------------------------------------------------
    void* Memory::Allocate(size_t size, Allocator* allocator)
    {
      return Allocate(size, kDefaultAlignment_, allocator);
    }

    //--------------------------------------------------------------------------
    void Memory::Deallocate(void* ptr)
    {
      if (ptr == nullptr)
      {
        return;
      }

      AllocationHeader* header = reinterpret_cast<AllocationHeader*>(
        PointerMath::Offset(ptr, -static_cast<int>(sizeof(AllocationHeader))));

      Allocator* alloc = header->allocator;

      alloc->Deallocate(PointerMath::Offset(
        header,
        -static_cast<int64_t>(header->align)
      ));
    }

    //--------------------------------------------------------------------------
    Memory::DefaultAllocator& Memory::default_allocator()
    {
      return default_allocator_;
    }
  }
}