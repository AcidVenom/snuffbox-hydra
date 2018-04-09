#include "scripting/duk/duk_allocator.h"

#include <foundation/memory/memory.h>
#include <foundation/auxiliary/logger.h>

namespace snuffbox
{
  namespace scripting
  {
    //--------------------------------------------------------------------------
    const size_t DukAllocator::kDefaultSize_ = 1024ul * 1024ul * 512ul;

    //--------------------------------------------------------------------------
    DukAllocator::DukAllocator(size_t max_size) :
      MallocAllocator(max_size)
    {

    }

    //--------------------------------------------------------------------------
    void* DukAllocator::DukAllocate(void* udata, size_t size)
    {
      DukAllocator* alloc;
      if (GetFromUserdata(udata, &alloc) == false)
      {
        return nullptr;
      }

      return foundation::Memory::Allocate(size, alloc);
    }

    //--------------------------------------------------------------------------
    void* DukAllocator::DukReallocate(void* udata, void* ptr, size_t size)
    {
      DukAllocator* alloc;
      if (GetFromUserdata(udata, &alloc) == false)
      {
        return nullptr;
      }

      size_t old_size = alloc->GetSize(ptr);
      void* block = foundation::Memory::Allocate(size, alloc);

      if (size >= old_size)
      {
        memcpy(block, ptr, old_size);
      }
      else
      {
        memcpy(block, ptr, size);
      }

      foundation::Memory::Deallocate(ptr);

      return block;
    }

    //--------------------------------------------------------------------------
    void DukAllocator::DukDeallocate(void* udata, void* ptr)
    {
      DukAllocator* alloc;
      if (GetFromUserdata(udata, &alloc) == false)
      {
        return;
      }

      foundation::Memory::Deallocate(ptr);
    }

    //--------------------------------------------------------------------------
    bool DukAllocator::GetFromUserdata(void* udata, DukAllocator** allocator)
    {
      if (udata == nullptr)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kScript,
          foundation::LogSeverity::kError,
          "Attempt to use a DukAllocator, but the userdata passed was nullptr");

        return false;
      }

      if (allocator == nullptr)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kScript,
          foundation::LogSeverity::kError,
          "Attempt to use a DukAllocator, but the out parameter for\
          the userdata was not specified");

        return false;
      }

      *allocator = reinterpret_cast<DukAllocator*>(udata);

      return true;
    }
  }
}