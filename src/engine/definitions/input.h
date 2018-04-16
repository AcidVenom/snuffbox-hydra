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
      kControllerButton,
      kMouseMove,
      kMouseScroll,
      kControllerAxis
    };

    /**
    * @brief The different states a key or button can be in
    *
    * kUp is not required, as it is simply not kDown
    */
    enum class KeyButtonState
    {
      kPressed,
      kReleased,
      kDown
    };
  }
}