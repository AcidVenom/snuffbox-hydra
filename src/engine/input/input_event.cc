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
      InputEvent(InputEventType::kKeyboardKey)
    {

    }

    //--------------------------------------------------------------------------
    InputMouseButtonEvent::InputMouseButtonEvent() :
      InputEvent(InputEventType::kMouseButton)
    {

    }

    //--------------------------------------------------------------------------
    InputControllerButtonEvent::InputControllerButtonEvent() :
      InputEvent(InputEventType::kControllerButton)
    {

    }

    //--------------------------------------------------------------------------
    InputMouseMoveEvent::InputMouseMoveEvent() :
      InputEvent(InputEventType::kMouseMove)
    {

    }

    //--------------------------------------------------------------------------
    InputMouseScrollEvent::InputMouseScrollEvent() :
      InputEvent(InputEventType::kMouseScroll)
    {

    }

    //--------------------------------------------------------------------------
    InputControllerAxisEvent::InputControllerAxisEvent() :
      InputEvent(InputEventType::kControllerAxis)
    {

    }
  }
}