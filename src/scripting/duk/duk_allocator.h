#pragma once

#include <foundation/memory/memory.h>

namespace snuffbox
{
  namespace scripting
  {
    /**
    * @brief This will redirect any calls duktape makes for memory allocation
    *        to snuffbox's allocators
    *
    * Using this allocator also makes sure that duktape has freed all its
    * memory after program execution and we can check how much memory the
    * allocator has in use. This allocator is simply a malloc allocator right
    * now, just like any other default allocator.
    *
    * @author Daniel Konings
    */
    class DukAllocator : public foundation::Memory::DefaultAllocator
    {

    public:

      /**
      * @see Memory::DefaultAllocator::DefaultAllocator
      *
      * The default size for the memory in the scripting environment is 512 MB
      */
      DukAllocator(size_t max_size = kDefaultSize_);

      /**
      * @brief The static version of this allocator's Allocate function that
      *        can be redirected to a duktape heap
      *
      * The udata parameter will contain a pointer to this allocator
      */
      static void* DukAllocate(void* udata, size_t size);

      /**
      * @brief The static version for reallocation that can be redirected 
      *        to a duktape heap
      *
      * The udata parameter will contain a pointer to this allocator.
      * This functions is naively implemented by simply deallocating the
      * previous pointer and allocating a new block with the new size.
      * The memory in the old block is copied into this newly allocated block
      */
      static void* DukReallocate(void* udata, void* ptr, size_t size);

      /**
      * @brief The static version of this allocator's Deallocate function that
      *        can be redirected to a duktape heap
      *
      * The udata parameter will contain a pointer to this allocator
      */
      static void DukDeallocate(void* udata, void* ptr);

      /**
      * @brief Retrieves a DukAllocator from userdata provided by duktape
      *
      * @param[in] udata The user data passed by duktape
      * @param[out] allocator The converted allocator
      *
      * @return Was the userdata valid?
      */
      static bool GetFromUserdata(void* udata, DukAllocator** allocator);

    private:

      static const size_t kDefaultSize_; //!< The default maximum size
    };
  }
}