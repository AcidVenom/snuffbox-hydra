#include "engine/input/mouse.h"
#include "engine/input/input_event.h"

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    Mouse::Mouse() :
      x_(0),
      y_(0),
      dx_(0),
      dy_(0),
      scroll_(0)
    {
      for (size_t i = 0; i < static_cast<size_t>(MouseButtons::kCount); ++i)
      {
        states_[i] = KeyButtonState::kUp;
      }
    }

    //--------------------------------------------------------------------------
    void Mouse::HandleKeyButtonEvent(int key_code, KeyButtonEvent evt)
    {
      if (key_code < 0 || key_code >= static_cast<int>(MouseButtons::kCount))
      {
        return;
      }

      KeyButtonState prev = states_[key_code];
      states_[key_code] = GetNewKeyState(states_[key_code], evt);
    }

    //--------------------------------------------------------------------------
    void Mouse::ResetPreviousStates()
    {
      for (size_t i = 0; i < static_cast<size_t>(MouseButtons::kCount); ++i)
      {
        KeyButtonState& state = states_[i];
        state = ResetPressedState(state);
      }

      dx_ = dy_ = scroll_ = 0;
    }

    //--------------------------------------------------------------------------
    KeyButtonState Mouse::GetKeyButtonState(int key_code) const
    {
      if (key_code < 0 || key_code >= static_cast<int>(MouseButtons::kCount))
      {
        return KeyButtonState::kError;
      }

      return states_[key_code];
    }

    //--------------------------------------------------------------------------
    void Mouse::HandleMouseMoveEvent(const InputMouseMoveEvent* evt)
    {
      int old_x = x_;
      int old_y = y_;

      x_ = evt->x;
      y_ = evt->y;

      dx_ = x_ - old_x;
      dy_ = y_ - old_y;
    }

    //--------------------------------------------------------------------------
    void Mouse::HandleMouseScrollEvent(const InputMouseScrollEvent* evt)
    {
      scroll_ = evt->delta;
    }

    //--------------------------------------------------------------------------
    void Mouse::GetMousePosition(int* x, int* y) const
    {
      if (x == nullptr || y == nullptr)
      {
        return;
      }

      *x = x_;
      *y = y_;
    }

    //--------------------------------------------------------------------------
    void Mouse::GetMouseMovement(int* dx, int* dy) const
    {
      if (dx == nullptr || dy == nullptr)
      {
        return;
      }

      *dx = dx_;
      *dy = dy_;
    }

    //--------------------------------------------------------------------------
    int Mouse::scroll() const
    {
      return scroll_;
    }
  }
}