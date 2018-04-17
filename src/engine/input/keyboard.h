#pragma once

#include "engine/definitions/keycodes.h"
#include "engine/definitions/input.h"
#include "engine/input/input_event_handler.h"

#include <cstddef>

namespace snuffbox
{
  namespace engine
  {
    /**
    * @brief The keyboard device that handles all key events from an InputFilter
    *
    * @author Daniel Konings
    */
    class Keyboard : public IInputEventHandler
    {

    public:

      /**
      * @brief Default constructor, changes all key states to
      *        KeyButtonState::kUp
      */
      Keyboard();

      /**
      * @see IInputEventHandler::HandleKeyButtonEvent
      */
      void HandleKeyButtonEvent(int key_code, KeyButtonEvent evt) override;

      /**
      * @see IInputEventHandler::ResetPreviousStates
      */
      void ResetPreviousStates() override;

      /**
      * @see IInputEventHandler::GetKeyButtonState
      */
      KeyButtonState GetKeyButtonState(int key_code) const override;

    private:

      /**
      * @brief The state of each possible key
      *
      * This array might have some invalid entries, as the key codes are not
      * contiguous. We simply ignore them.
      */
      KeyButtonState states_[static_cast<size_t>(Keys::kCount)];
    };
  }
}