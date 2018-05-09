#pragma once

#include "foundation/memory/allocators/allocator.h"

namespace snuffbox
{
  namespace foundation
  {
    /**
    * @brief A "malloc" allocator, uses regular malloc/free
    *
    * This allocator guarantees that the underlying memory allocation is done
    * properly. If any issues arise with the memory interface, they can be found
    * with this specific allocator. As long as memory is not a bottleneck, this
    * memory allocator should be used.
    *
    * @author Daniel Konings
    */
    class MallocAllocator : public IAllocator
    {

    public:

      /**
      * @brief Initializes this allocator for use
      * 
      * @param[in] max_size The maximum size of this allocator
      */
      MallocAllocator(size_t max_size);

    protected:

      /**
      * @brief Used to keep track of an allocation done with this allocator
      */
      struct AllocationHeader
      {
        size_t size; //!< The size of the allocation
      };

      /**
      * @see IAllocator::Allocate
      *
      * Align is currently unused with this allocator
      */
      void* AllocateImpl(size_t size, size_t align) override;

      /**
      * @see IAllocator::Deallocate
      */
      size_t DeallocateImpl(void* ptr) override;

      /**
      * @brief Retrieves the size of the memory block at a pointer from
      *        the allocation header
      *
      * @param[in] ptr The pointer to the memory block
      *
      * @return The size of the memory block
      */
      size_t GetSize(void* ptr);
    };
  }
}