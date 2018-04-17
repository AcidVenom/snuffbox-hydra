#include "engine/input/input_event.h"

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    InputEvent::InputEvent(InputEventType t) :
      type(t)
    {

    }

    //--------------------------------------------------------------------------
    InputKeyboardKeyEvent::InputKeyboardKeyEvent() :
      InputEvent(InputEventType::kKeyboardKey),
      key_code(Keys::kUnknown)
    {

    }

    //--------------------------------------------------------------------------
    InputMouseButtonEvent::InputMouseButtonEvent() :
      InputEvent(InputEventType::kMouseButton),
      button(MouseButtons::kUnknown)
    {

    }

    //--------------------------------------------------------------------------
    InputControllerButtonEvent::InputControllerButtonEvent() :
      InputEvent(InputEventType::kControllerButton),
      button(ControllerButtons::kUnknown)
    {

    }

    //--------------------------------------------------------------------------
    InputMouseMoveEvent::InputMouseMoveEvent() :
      InputEvent(InputEventType::kMouseMove),
      dx(0),
      dy(0)
    {

    }

    //--------------------------------------------------------------------------
    InputMouseScrollEvent::InputMouseScrollEvent() :
      InputEvent(InputEventType::kMouseScroll),
      delta(0)
    {

    }

    //--------------------------------------------------------------------------
    InputControllerAxisEvent::InputControllerAxisEvent() :
      InputEvent(InputEventType::kControllerAxis),
      axis(ControllerAxes::kUnknown)
    {

    }
  }
}