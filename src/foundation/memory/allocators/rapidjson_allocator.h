#pragma once

#include "foundation/memory/allocators/malloc_allocator.h"

#include <rapidjson/allocators.h>

namespace snuffbox
{
  namespace foundation
  {
    /**
    * @brief Used to provide custom allocator functionality to the
    *        rapidjson dependency
    *
    * @see http://rapidjson.org/classrapidjson_1_1_allocator.html#details
    *
    * @author Daniel Konings
    */
    class RapidJsonStackAllocator : public MallocAllocator
    {

    public:

      /**
      * @see MallocAllocator::MallocAllocator
      */
      RapidJsonStackAllocator();

      /**
      * @brief Allocate a block of memory
      *
      * @param[in] size The size of the memory block to allocate
      *
      * @return The pointer to the allocated block of memory, or nullptr
      *         if failed
      */
      void* Malloc(size_t size);

      /**
      * @brief Resizes a block of memory
      *
      * @param[in] original The original pointer of the block to resize
      * @param[in] original_size The original size of the memory block
      * @param[in] new_size The new size of the memory block
      *
      * @return The resized memory block, or nullptr if failed
      */
      void* Realloc(void* original, size_t original_size, size_t new_size);

      /**
      * @brief Frees a block of memory
      *
      * @param[in] ptr The memory block to free
      */
      static void Free(void* ptr);

      /**
      * @brief Whether this allocator needs to call Free()
      */
      static const bool kNeedFree;

    private:

      static const size_t kMaxSize_; //!< The max bytes that can be allocated
    };

    /**
    * @brief Re-uses rapidjson's MemoryPoolAllocator by specifying the
    *        RapidJsonAllocator as base allocator
    */
    using RapidJsonAllocator = 
      rapidjson::MemoryPoolAllocator<RapidJsonStackAllocator>;
  }
}