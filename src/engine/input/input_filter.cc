#include "engine/input/input_filter.h"
#include "engine/input/input_event.h"

#include <foundation/memory/memory.h>
#include <foundation/auxiliary/pointer_math.h>

#include <cinttypes>
#include <cassert>

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    const size_t IInputFilter::kEventSize_ = 16;

    //--------------------------------------------------------------------------
    IInputFilter::IInputFilter(size_t max_buffered_events) :
      buffer_(nullptr),
      buffer_size_(max_buffered_events),
      num_events_(0)
    {
      size_t full_size = kEventSize_ * max_buffered_events;
      void* block = foundation::Memory::Allocate(full_size);

      buffer_ = reinterpret_cast<InputEvent*>(block);
    }

    //--------------------------------------------------------------------------
    bool IInputFilter::GetNext(const InputEvent** e)
    {
      if (e == nullptr || num_events_ == 0)
      {
        return false;
      }

      size_t idx = (num_events_ - 1) * kEventSize_;

      void* block = 
        foundation::PointerMath::Offset(buffer_, static_cast<int64_t>(idx));

      *e = reinterpret_cast<const InputEvent*>(block);

      --num_events_;

      return true;
    }

    //--------------------------------------------------------------------------
    void IInputFilter::BufferEvent(const InputEvent* e)
    {
      if (num_events_ + 1 >= buffer_size_)
      {
        return;
      }

      size_t size = 0;

      switch (e->type)
      {
      case InputEventType::kKeyboardKey:
        size = sizeof(InputKeyboardKeyEvent);
        break;

      case InputEventType::kMouseButton:
        size = sizeof(InputMouseButtonEvent);
        break;

      case InputEventType::kJoystickButton:
        size = sizeof(InputJoystickButtonEvent);
        break;

      case InputEventType::kMouseMove:
        size = sizeof(InputMouseMoveEvent);
        break;

      case InputEventType::kMouseScroll:
        size = sizeof(InputMouseScrollEvent);
        break;

      case InputEventType::kJoystickAxis:
        size = sizeof(InputJoystickAxisEvent);
        break;

      default:
        break;
      }

      assert(size <= kEventSize_ && "Input event doesn't fit in event buffer");

      if (size == 0 || size > kEventSize_)
      {
        return;
      }

      size_t offset = kEventSize_ * num_events_;

      void* block = 
        foundation::PointerMath::Offset(buffer_, static_cast<int64_t>(offset));

      memcpy(block, e, size);

      ++num_events_;
    }

    //--------------------------------------------------------------------------
    IInputFilter::~IInputFilter()
    {
      if (buffer_ != nullptr)
      {
        foundation::Memory::Deallocate(buffer_);
      }

      buffer_ = nullptr;
      buffer_size_ = 0;
      num_events_ = 0;
    }
  }
}