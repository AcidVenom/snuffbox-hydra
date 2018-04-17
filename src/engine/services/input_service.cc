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
      for (size_t i = 0; i < kNumSupportedJoysticks; ++i)
      {
        joysticks_[i] = Joystick(static_cast<int>(i));
      }
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
    void InputService::HandleJoystickButtonEvent(
      const InputJoystickButtonEvent* evt)
    {
      int id = evt->id;

      if (id < 0 || id >= kNumSupportedJoysticks)
      {
        return;
      }

      joysticks_[id].HandleKeyButtonEvent(
        static_cast<int>(evt->button), 
        evt->evt);
    }

    //--------------------------------------------------------------------------
    void InputService::HandleMouseMoveEvent(const InputMouseMoveEvent* evt)
    {
      mouse_.HandleMouseMoveEvent(evt);
    }

    //--------------------------------------------------------------------------
    void InputService::HandleMouseScrollEvent(const InputMouseScrollEvent* evt)
    {
      mouse_.HandleMouseScrollEvent(evt);
    }

    //--------------------------------------------------------------------------
    void InputService::HandleJoystickConnectEvent(
      const InputJoystickConnectEvent* evt)
    {
      int id = evt->id;

      if (id < 0 || id >= kNumSupportedJoysticks)
      {
        return;
      }

      joysticks_[id].HandleJoystickConnectEvent(evt);
    }

    //--------------------------------------------------------------------------
    void InputService::HandleJoystickAxisEvent(
      const InputJoystickAxisEvent* evt)
    {
      int id = evt->id;

      if (id < 0 || id >= kNumSupportedJoysticks)
      {
        return;
      }

      joysticks_[id].HandleJoystickAxisEvent(evt);
    }

    //--------------------------------------------------------------------------
    void InputService::Flush()
    {
      keyboard_.ResetPreviousStates();
      mouse_.ResetPreviousStates();

      for (size_t i = 0; i < kNumSupportedJoysticks; ++i)
      {
        joysticks_[i].ResetPreviousStates();
      }

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

          case InputEventType::kJoystickButton:
            HandleJoystickButtonEvent(
              static_cast<const InputJoystickButtonEvent*>(e));
            break;

          case InputEventType::kMouseMove:
            HandleMouseMoveEvent(
              static_cast<const InputMouseMoveEvent*>(e));
            break;

          case InputEventType::kMouseScroll:
            HandleMouseScrollEvent(
              static_cast<const InputMouseScrollEvent*>(e));
            break;

          case InputEventType::kJoystickConnect:
            HandleJoystickConnectEvent(
              static_cast<const InputJoystickConnectEvent*>(e));
            break;

          case InputEventType::kJoystickAxis:
            HandleJoystickAxisEvent(
              static_cast<const InputJoystickAxisEvent*>(e));
            break;

          default:
            break;
          }
        }
      }
    }
  }
}