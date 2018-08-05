#include "engine/input/input_event_handler.h"

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    KeyButtonState IInputEventHandler::GetNewKeyState(
      KeyButtonState prev,
      KeyButtonEvent evt) const
    {
      if (prev == KeyButtonState::kPressed || prev == KeyButtonState::kDown)
      {
        switch (evt)
        {
        case KeyButtonEvent::kReleased:
          return KeyButtonState::kReleased;

        case KeyButtonEvent::kPressed:
          return KeyButtonState::kDown;

        default:
          return KeyButtonState::kUp;
        }
      }
      else if (prev == KeyButtonState::kReleased || prev == KeyButtonState::kUp)
      {
        switch (evt)
        {
        case KeyButtonEvent::kPressed:
          return KeyButtonState::kPressed;

        case KeyButtonEvent::kReleased:
          return KeyButtonState::kUp;
          
        default:
          return KeyButtonState::kDown;
        }
      }

      return KeyButtonState::kUp;
    }

    //--------------------------------------------------------------------------
    KeyButtonState IInputEventHandler::ResetPressedState(
      KeyButtonState state) const
    {
      switch (state)
      {
      case KeyButtonState::kPressed:
        return KeyButtonState::kDown;

      case KeyButtonState::kReleased:
        return KeyButtonState::kUp;

      default:
        return state;
      }
    }
  }
}