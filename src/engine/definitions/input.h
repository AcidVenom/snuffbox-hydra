#pragma once

namespace snuffbox
{
  namespace engine
  {
    /**
    * @brief The different input events that exist
    */
    enum class InputEventType
    {
      kKeyboardKey,
      kMouseButton,
      kJoystickButton,
      kMouseMove,
      kMouseScroll,
      kJoystickAxis,
      kJoystickConnect
    };

    /**
    * @brief The different events that can be fired for a key or button
    */
    enum class KeyButtonEvent
    {
      kPressed,
      kReleased
    };

    /**
    * @brief The different states that a key or button can be in
    */
    enum class KeyButtonState
    {
      kPressed, //!< The state for when a key or button is pressed this frame
      kReleased, //!< The state for when a key or button is released this frame
      kDown, //!< When a button is being held
      kUp, //!< Idle state
      kError //!< An error state if a key was requested, but out of bounds
    };
  }
}