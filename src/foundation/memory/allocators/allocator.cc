#include "foundation/memory/allocators/allocator.h"
#include "foundation/auxiliary/logger.h"

namespace snuffbox
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    IAllocator::IAllocator(size_t max_size) :
      max_size_(max_size),
      open_allocations_(0),
      allocated_(0)
    {

    }

    //--------------------------------------------------------------------------
    size_t IAllocator::open_allocations() const
    {
      return open_allocations_;
    }

    //--------------------------------------------------------------------------
    size_t IAllocator::allocated() const
    {
      return allocated_;
    }

    //--------------------------------------------------------------------------
    IAllocator::~IAllocator()
    {
      assert(open_allocations_ == 0 && allocated_ == 0 &&
        "Memory leak detected in an allocator");
    }

    //--------------------------------------------------------------------------
    void* IAllocator::Allocate(size_t size, size_t align)
    {
      std::lock_guard<std::recursive_mutex> lock(mutex_);

      if (allocated_ + size > max_size_)
      {
        Logger::Assert(false, "Buffer overflow in allocator");
        return nullptr;
      }

      allocated_ += size;
      ++open_allocations_;

      return AllocateImpl(size, align);
    }

    //--------------------------------------------------------------------------
    size_t IAllocator::Deallocate(void* ptr)
    {
      std::lock_guard<std::recursive_mutex> lock(mutex_);

      size_t deallocated = DeallocateImpl(ptr);

      if (max_size_ < deallocated)
      {
        Logger::Assert(false, 
          "Attempted to deallocate more than was ever allocated");
        return 0;
      }

      allocated_ -= deallocated;
      --open_allocations_;

      return deallocated;
    }
  }
}