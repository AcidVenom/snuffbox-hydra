#include "engine/input/keyboard.h"

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    Keyboard::Keyboard()
    {
      for (size_t i = 0; i < static_cast<size_t>(Keys::kCount); ++i)
      {
        states_[i] = KeyButtonState::kUp;
      }
    }

    //--------------------------------------------------------------------------
    void Keyboard::HandleKeyButtonEvent(int key_code, KeyButtonEvent evt)
    {
      if (key_code < 0 || key_code >= static_cast<int>(Keys::kCount))
      {
        return;
      }

      KeyButtonState prev = states_[key_code];
      states_[key_code] = GetNewKeyState(prev, evt);
    }

    //--------------------------------------------------------------------------
    void Keyboard::ResetPreviousStates()
    {
      for (size_t i = 0; i < static_cast<size_t>(Keys::kCount); ++i)
      {
        KeyButtonState& state = states_[i];
        state = ResetPressedState(state);
      }
    }

    //--------------------------------------------------------------------------
    KeyButtonState Keyboard::GetKeyButtonState(int key_code) const
    {
      if (key_code < 0 || key_code >= static_cast<int>(Keys::kCount))
      {
        return KeyButtonState::kError;
      }

      return states_[key_code];
    }
  }
}