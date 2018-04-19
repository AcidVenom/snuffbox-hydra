#pragma once

#include "engine/input/input_event_handler.h"
#include "engine/definitions/keycodes.h"

namespace snuffbox
{
  namespace engine
  {
    struct InputJoystickConnectEvent;
    struct InputJoystickAxisEvent;

    /**
    * @brief The joystick device that handles all key events from an InputFilter
    *
    * Joysticks are a special case in the sense that there can be a multitude
    * of joysticks connected. Every joystick event should also contain the
    * corresponding joystick ID.
    *
    * @author Daniel Konings
    */
    class Joystick : public IInputEventHandler
    {

    public:

      /**
      * @brief Default constructor, sets the ID to -1
      */
      Joystick();

      /**
      * @brief Construct through a joystick ID
      *
      * @param[in] id The ID to assign to this joystick
      */
      Joystick(int id);

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

      /**
      * @see IInputEventHandler::HandleJoystickConnectEvent
      */
      void HandleJoystickConnectEvent(const InputJoystickConnectEvent* evt);

      /**
      * @see IInputEventHandler::HandleJoystickAxisEvent
      */
      void HandleJoystickAxisEvent(const InputJoystickAxisEvent* evt);

      /**
      * @brief Retrieves the value of a specified axis
      *
      * @param[in] axis The axis to retrieve the value of
      *
      * @return The value of the axis, between -1.0 and 1.0
      */
      float GetAxis(JoystickAxes axis) const;

      /**
      * @return Is this joystick connected?
      */
      bool connected() const;

    protected:

      /**
      * @brief Called when a joystick is connected at this joystick's ID
      *
      * @param[in] device_name The device name from the connect event
      */
      void OnConnect(const char* device_name);

      /**
      * @brief Called when a joystick is disconnected at this joystick's ID
      */
      void OnDisconnect();

      /**
      * @brief Resets all states of this joystick
      *
      * KeyButtonState values of the buttons will be set to KeyButtonState::kUp
      * All axes will be reset to 0.0
      */
      void Reset();

    private:

      int id_; //!< The ID of this joystick
      bool connected_; //!< Is this joystick connected?

      /**
      * @brief The states for each button on the joystick
      */
      KeyButtonState states_[static_cast<size_t>(JoystickButtons::kCount)];

      /**
      * @brief The current value between -1.0 and 1.0 per axis
      */
      float axes_[static_cast<size_t>(JoystickAxes::kCount)];
    };
  }
}