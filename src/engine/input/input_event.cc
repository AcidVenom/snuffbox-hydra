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
    InputJoystickButtonEvent::InputJoystickButtonEvent() :
      InputEvent(InputEventType::kJoystickButton),
      button(JoystickButtons::kUnknown)
    {

    }

    //--------------------------------------------------------------------------
    InputMouseMoveEvent::InputMouseMoveEvent() :
      InputEvent(InputEventType::kMouseMove),
      x(0),
      y(0)
    {

    }

    //--------------------------------------------------------------------------
    InputMouseScrollEvent::InputMouseScrollEvent() :
      InputEvent(InputEventType::kMouseScroll),
      delta(0)
    {

    }

    //--------------------------------------------------------------------------
    InputJoystickAxisEvent::InputJoystickAxisEvent() :
      InputEvent(InputEventType::kJoystickAxis),
      axis(JoystickAxes::kUnknown)
    {

    }
  }
}