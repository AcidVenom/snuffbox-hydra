#pragma once

#include "engine/definitions/input.h"

namespace snuffbox
{
  namespace engine
  {
    /**
    * @brief The base class for every input device to use
    *
    * This class is here to process key and button events, so that the
    * appropriate states are set based on the event received. This behavior
    * is the same for every key and/or button, this class provides that
    * functionality.
    *
    * @author Daniel Konings
    */
    class IInputEventHandler
    {

    public:

      /**
      * @brief Handle a key or button event using InputEventHandler::GetNewState
      *
      * @param[in] key_code The corresponding key code for the event
      * @param[in] evt The type of key/button event that was fired
      */
      virtual void HandleKeyButtonEvent(int key_code, KeyButtonEvent evt) = 0;

      /**
      * @brief Resets any one-frame states
      */
      virtual void ResetPreviousStates() = 0;

      /**
      * @brief Retrieves the current key/button state of a key code
      *
      * This should be implemented in the derived classes to make sure
      * that the requested key code is in bounds.
      *
      * @param[in] key_code The requested key code
      *
      * @return The state for the key code
      */
      virtual KeyButtonState GetKeyButtonState(int key_code) const = 0;

    protected:

      /**
      * @brief Compares a new key event with a previous state and provides
      *        the new state that the key will be in this frame accordingly
      * 
      * This function should be called from the implementation of
      * InputEventHandler::HandleKeyButtonEvent in the derived classes.
      *
      * Possible cases are:
      *
      * prev was KeyButtonState::kUp and evt is KeyButtonEvent::kPressed:
      * -> returns KeyButtonState::kPressed
      *
      * prev was KeyButtonState::kDown or KeyButtonState::kPressed 
      * and evt is KeyButtonEvent::kReleased:
      * -> returns KeyButtonState::kReleased
      *
      */
      KeyButtonState GetNewKeyState(
        KeyButtonState prev, 
        KeyButtonEvent evt) const;

      /**
      * @brief Resets a pressed/released state to the correct state
      *
      * @param[in] prev The previous state 
      *                 (should be either down, pressed, released or up)
      *
      * @return The new state
      */
      KeyButtonState ResetPressedState(KeyButtonState prev) const;
    };
  }
}