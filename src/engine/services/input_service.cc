#include "engine/services/input_service.h"
#include "engine/input/input_filter.h"
#include "engine/input/input_event.h"

#include "engine/auxiliary/debug.h"

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    InputService::InputService() :
      ServiceBase("InputService")
    {

    }

    //--------------------------------------------------------------------------
    foundation::ErrorCodes InputService::OnInitialize(Application& app)
    {
      return foundation::ErrorCodes::kSuccess;
    }

    //--------------------------------------------------------------------------
    void InputService::OnShutdown(Application& app)
    {

    }

    //--------------------------------------------------------------------------
    void InputService::RegisterInputFilter(IInputFilter* filter)
    {
      if (filter == nullptr)
      {
        return;
      }

      for (size_t i = 0; i < filters_.size(); ++i)
      {
        if (filters_.at(i) == filter)
        {
          Debug::LogVerbosity<1>(
            foundation::LogSeverity::kWarning,
            "Attempted to register a duplicate input filter");

          return;
        }
      }

      filters_.push_back(filter);
    }

    //--------------------------------------------------------------------------
    void InputService::RemoveInputFilter(IInputFilter* filter)
    {
      for (size_t i = 0; i < filters_.size(); ++i)
      {
        if (filters_.at(i) == filter)
        {
          filters_.erase(filters_.begin() + i);
          break;
        }
      }
    }

    //--------------------------------------------------------------------------
    void InputService::HandleKeyboardKeyEvent(const InputKeyboardKeyEvent* evt)
    {
      keyboard_.HandleKeyButtonEvent(
        static_cast<int>(evt->key_code), 
        evt->evt);
    }

    //--------------------------------------------------------------------------
    void InputService::HandleMouseButtonEvent(const InputMouseButtonEvent* evt)
    {
      mouse_.HandleKeyButtonEvent(
        static_cast<int>(evt->button), 
        evt->evt);
    }

    //--------------------------------------------------------------------------
    void InputService::HandleMouseMoveEvent(const InputMouseMoveEvent* evt)
    {
      mouse_.HandleMouseMoveEvent(evt);
    }

    //--------------------------------------------------------------------------
    void InputService::Flush()
    {
      keyboard_.ResetPreviousStates();
      mouse_.ResetPreviousStates();

      IInputFilter* filter;
      const InputEvent* e;

      for (size_t i = 0; i < filters_.size(); ++i)
      {
        filter = filters_.at(i);

        while (filter->GetNext(&e) == true)
        {
          switch (e->type)
          {
          case InputEventType::kKeyboardKey:
            HandleKeyboardKeyEvent(
              static_cast<const InputKeyboardKeyEvent*>(e));
            break;

          case InputEventType::kMouseButton:
            HandleMouseButtonEvent(
              static_cast<const InputMouseButtonEvent*>(e));
            break;

          case InputEventType::kMouseMove:
            HandleMouseMoveEvent(
              static_cast<const InputMouseMoveEvent*>(e));
            break;
          }
        }
      }
    }
  }
}