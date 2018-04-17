#pragma once

#include "engine/definitions/input.h"
#include "engine/definitions/keycodes.h"

#include <foundation/containers/string.h>

#include <cinttypes>

namespace snuffbox
{
  namespace engine
  {
    /**
    * @brief The base class for every input event to use
    *
    * @author Daniel Konings
    */
    struct InputEvent
    {
      /**
      * @brief Construct an event by type
      *
      * @param[in] type The type of this input event
      */
      InputEvent(InputEventType t);

      InputEventType type; //!< The type of this event
    };

    /**
    * @brief An event to be created when a keyboard key is pressed
    *
    * @author Daniel Konings
    */
    struct InputKeyboardKeyEvent : public InputEvent
    {
      /**
      * @see InputEvent::InputEvent
      */
      InputKeyboardKeyEvent();

      KeyButtonEvent evt; //!< The event that was fired for the key
      Keys key_code; //!< The affected key code
    };

    /**
    * @brief An event to be created when a mouse button is pressed
    *
    * @author Daniel Konings
    */
    struct InputMouseButtonEvent : public InputEvent
    {
      /**
      * @see InputEvent::InputEvent
      */
      InputMouseButtonEvent();

      KeyButtonEvent evt; //!< The event that was fired for the button
      MouseButtons button; //!< The affected button code
    };

    /**
    * @brief An event to be created when a joystick button is pressed
    *
    * @author Daniel Konings
    */
    struct InputJoystickButtonEvent : public InputEvent
    {
      /**
      * @see InputEvent::InputEvent
      */
      InputJoystickButtonEvent();

      uint8_t id; //!< The joystick ID this event corresponds to
      KeyButtonEvent evt; //!< The event that was fired for the button
      JoystickButtons button; //!< The affected button code
    };

    /**
    * @brief An event to be created when the mouse is moved to a new position
    *
    * @author Daniel Konings
    */
    struct InputMouseMoveEvent : public InputEvent
    {
      /**
      * @see InputEvent::InputEvent
      */
      InputMouseMoveEvent();

      int x; //!< The position on the horizontal axis, in pixels
      int y; //!< The position on the vertical axis, in pixels
    };

    /**
    * @brief An event to be created when the mouse wheel is scrolled
    *
    * @author Daniel Konings
    */
    struct InputMouseScrollEvent : public InputEvent
    {
      /**
      * @see InputEvent::InputEvent
      */
      InputMouseScrollEvent();

      int delta; //!< The delta, a negative delta represents scrolling down
    };

    /**
    * @brief An event to be created when any of the joystick's axes have
    *        changed
    *
    * @author Daniel Konings
    */
    struct InputJoystickAxisEvent : public InputEvent
    {
      /**
      * @see InputEvent::InputEvent
      */
      InputJoystickAxisEvent();

      uint8_t id; //!< The joystick ID this event corresponds to
      JoystickAxes axis; //!< The affected axis
      float value; //!< A value in the -1.0 to 1.0 range
    };

    /**
    * @brief Joystick connect event, called when a joystick is connected to or
    *        disconnected from the host machine
    *
    * @author Daniel Konings
    */
    struct InputJoystickConnectEvent : public InputEvent
    {
      /**
      * @see InputEvent::InputEvent
      */
      InputJoystickConnectEvent();

      uint8_t id; //!< The joystick ID this event corresponds to
      bool connected; //!< Is the joystick connected or disconnected?
      const char* device_name; //!< The device name of the joystick
    };
  }
}