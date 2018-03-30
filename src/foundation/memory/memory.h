#pragma once

#include "foundation/memory/allocators/malloc_allocator.h"

#include <EASTL/memory.h>

namespace snuffbox
{
  namespace foundation
  {
    /**
    * @brief The memory interface to be used throughout the engine
    * 
    * "new" and "delete" should be used nowhere, this interface is here to
    * guarantee that. The interface uses the custom memory allocators provided
    * by the user and has a default allocator fallback if not specified.
    *
    * @see Allocator
    *
    * @author Daniel Konings
    */
    class Memory
    {
      
    public:

      /**
      * @brief The default allocator type
      */
      using DefaultAllocator = MallocAllocator;

      /**
      * @brief Allocates a block of memory with a specified size and alignment
      *
      * This function also allocates a Memory::AllocationHeader,
      * the header is used to store a reference to the allocator that was used
      * to allocate the memory block. During deallocation using
      * Memory::Deallocate this allocator will be used.
      *
      * @param[in] size The size to allocate
      * @param[in] align The alignment to use
      * @param[in] allocator The allocator to use for the allocation
      *
      * @return The pointer to the address of the allocated memory block
      */
      static void* Allocate(
        size_t size, 
        size_t align, 
        Allocator* allocator = &default_allocator());

      /**
      * @see Memory::Allocate
      *
      * Uses the default alignment specified in Memory::kDefaultAlignment_
      */
      static void* Allocate(
        size_t size,
        Allocator* allocator = &default_allocator());

      /**
      * @brief Deallocates a memory block and its respective header
      *
      * @see Memory::Allocate
      *
      * @param[in] ptr The pointer to the memory block to deallocate
      */
      static void Deallocate(void* ptr);

      /**
      * @brief Constructs a class using and calls its constructor
      *
      * The equivalent of "new" to use instead
      *
      * @tparam T The type to construct
      * @tparam Args... The arguments to pass into the constructor
      *
      * @param[in] allocator The allocator to use
      * @param[in] args The arguments to pass into the constructor
      *
      * @return The constructed object pointer
      */
      template <typename T, typename ... Args>
      static T* Construct(Allocator* allocator, Args&&... args);

      /**
      * @see Memory::Construct
      *
      * @brief The default allocator version for construction
      */
      template <typename T, typename ... Args>
      static T* Construct(Args&&... args);

      /**
      * @brief Destructs a class and calls its destructor
      *
      * @tparam T The type to destruct
      *
      * @param[in] ptr The pointer to the constructed object
      */
      template <typename T>
      static void Destruct(T* ptr);

      /**
      * @return The default allocator
      */
      static DefaultAllocator& default_allocator();

    protected:

      /**
      * @brief An allocation header to store allocation information
      *
      * @author Daniel Konings
      */
      struct AllocationHeader
      {
        Allocator* allocator; //!< The allocator used for the allocation
        size_t align; //!< The alignment used during the allocation
        size_t size; //!< The size of the allocation
      };

    private:

      static const size_t kDefaultHeapSize_; //!< The default heap size
      static const size_t kDefaultAlignment_; //!< The default alignment

      /**
      * @brief The default allocator, 
      *        it is initialized with Memory::kDefaultHeapSize_
      */
      static DefaultAllocator default_allocator_;
    };

    //--------------------------------------------------------------------------
    template <typename T, typename ... Args>
    inline T* Memory::Construct(Allocator* allocator, Args&&... args)
    {
      T* allocated = reinterpret_cast<T*>(Allocate(sizeof(T), allocator));
      new (allocated) T(eastl::forward<Args>(args)...);

      return allocated;
    }

    //--------------------------------------------------------------------------
    template <typename T, typename ... Args>
    inline T* Memory::Construct(Args&&... args)
    {
      return Construct<T, Args...>(
        &default_allocator(),
        eastl::forward<Args>(args)...);
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void Memory::Destruct(T* ptr)
    {
      ptr->~T();
      Deallocate(ptr);
    }
  }
}