#include "engine/input/joystick.h"
#include "engine/input/input_event.h"

#include "engine/auxiliary/debug.h"

#include <cassert>

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    Joystick::Joystick() :
      id_(-1),
      connected_(false)
    {
      Reset();
    }

    //--------------------------------------------------------------------------
    Joystick::Joystick(int id) :
      id_(id),
      connected_(false)
    {
      Reset();
    }

    //--------------------------------------------------------------------------
    void Joystick::HandleKeyButtonEvent(int key_code, KeyButtonEvent evt)
    {
      if (key_code < 0 || key_code >= static_cast<int>(JoystickButtons::kCount))
      {
        return;
      }

      KeyButtonState prev = states_[key_code];
      states_[key_code] = GetNewKeyState(prev, evt);
    }

    //--------------------------------------------------------------------------
    void Joystick::ResetPreviousStates()
    {
      for (size_t i = 0; i < static_cast<size_t>(JoystickButtons::kCount); ++i)
      {
        KeyButtonState& state = states_[i];
        state = ResetPressedState(state);
      }
    }

    //--------------------------------------------------------------------------
    KeyButtonState Joystick::GetKeyButtonState(int key_code) const
    {
      if (
        key_code < 0 || 
        key_code >= static_cast<int>(JoystickButtons::kCount) ||
        connected_ == false)
      {
        return KeyButtonState::kError;
      }

      return states_[key_code];
    }

    //--------------------------------------------------------------------------
    void Joystick::HandleJoystickConnectEvent(
      const InputJoystickConnectEvent* evt)
    {
      assert(evt->id == id_ && "Joystick event ID doesn't match actual ID");

      connected_ = evt->connected;

      if (connected_ == true)
      {
        OnConnect(evt->device_name);
      }
      else
      {
        OnDisconnect();
      }

      Reset();
    }

    //--------------------------------------------------------------------------
    void Joystick::HandleJoystickAxisEvent(const InputJoystickAxisEvent* evt)
    {
      assert(evt->id == id_ && "Joystick event ID doesn't match actual ID");

      if (connected_ == false)
      {
        return;
      }

      int axis = static_cast<int>(evt->axis);

      if (axis < 0 || axis >= static_cast<int>(JoystickAxes::kCount))
      {
        return;
      }

      axes_[axis] = evt->value;
    }

    //--------------------------------------------------------------------------
    float Joystick::GetAxis(JoystickAxes axis)
    {
      if (connected_ == false)
      {
        return 0.0f;
      }

      return axes_[static_cast<size_t>(axis)];
    }

    //--------------------------------------------------------------------------
    bool Joystick::connected() const
    {
      return connected_;
    }

    //--------------------------------------------------------------------------
    void Joystick::OnConnect(const char* device_name)
    {
      Debug::LogVerbosity<1>(
        foundation::LogSeverity::kInfo, 
        "Controller ({0}) connected: {1}",
        id_,
        device_name);
    }

    //--------------------------------------------------------------------------
    void Joystick::OnDisconnect()
    {
      Debug::LogVerbosity<1>(
        foundation::LogSeverity::kInfo, 
        "Controller ({0}) disconnected",
        id_);
    }

    //--------------------------------------------------------------------------
    void Joystick::Reset()
    {
      for (size_t i = 0; i < static_cast<size_t>(JoystickButtons::kCount); ++i)
      {
        states_[i] = KeyButtonState::kUp;
      }

      for (size_t i = 0; i < static_cast<size_t>(JoystickAxes::kCount); ++i)
      {
        axes_[i] = 0.0f;
      }

      axes_[static_cast<size_t>(JoystickAxes::kLeftTrigger)] = -1.0f;
      axes_[static_cast<size_t>(JoystickAxes::kRightTrigger)] = -1.0f;
    }
  }
}