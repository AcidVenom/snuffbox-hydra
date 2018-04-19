#pragma once

#include "engine/services/service.h"

#include "engine/input/keyboard.h"
#include "engine/input/mouse.h"
#include "engine/input/joystick.h"

#include <foundation/containers/vector.h>

#include <scripting/script_class.h>

#include <glm/glm.hpp>

namespace snuffbox
{
  namespace engine
  {
    class IInputFilter;

    struct InputKeyboardKeyEvent;
    struct InputMouseButtonEvent;
    struct InputMouseMoveEvent;
    struct InputMouseScrollEvent;
    struct InputJoystickButtonEvent;
    struct InputJoystickAxisEvent;

    /**
    * @brief An input service to read input data from, that can be received
    *        from a multitude of input filters. 
    
    *        The input service only reads the event data and sets 
    *        keyboard/mouse/controller states accordingly. 
    *        This makes it so that we can have multiple native message queues
    *        sending over input data. (e.g. Qt's message loop and GLFW's)
    *
    * @see IInputFilter
    *
    * @author Daniel Konings
    */
    SCRIPT_CLASS() class InputService : 
      public ServiceBase<InputService>,
      public scripting::ScriptClass
    {

    public:

      SCRIPT_NAME(Input);

      /**
      * @see ServiceBase::ServiceBase
      */
      InputService();

      /**
      * @see IService::OnInitialize
      */
      foundation::ErrorCodes OnInitialize(Application& app) override;

      /**
      * @see IService::OnShutdown
      */
      void OnShutdown(Application& app) override;

      /**
      * @brief Registers an input filter to read input data from
      *
      * @param[in] filter The filter to register
      */
      void RegisterInputFilter(IInputFilter* filter);

      /**
      * @brief Removes an input filter
      *
      * @param[in] filter The filter to remove
      */
      void RemoveInputFilter(IInputFilter* filter);

      /**
      * @brief Handles a keyboard key event by sending it to
      *        InputService::keyboard_
      *
      * @param[in] evt The event to handle
      */
      void HandleKeyboardKeyEvent(const InputKeyboardKeyEvent* evt);

      /**
      * @brief Handles a mouse button event by sending it to
      *        InputService::mouse_
      *
      * @param[in] evt The event to handle
      */
      void HandleMouseButtonEvent(const InputMouseButtonEvent* evt);

      /**
      * @brief Handles a joystick button event by sending it to the
      *        correct joystick based on evt->id
      *
      * @param[in] evt The event to handle
      */
      void HandleJoystickButtonEvent(const InputJoystickButtonEvent* evt);

      /**
      * @brief Handles a mouse move event by sending it to
      *        InputService::mouse_
      *
      * @param[in] evt The event to handle
      */
      void HandleMouseMoveEvent(const InputMouseMoveEvent* evt);

      /**
      * @brief Handles a mouse scroll event by sending it to
      *        InputService::mouse_
      *
      * @param[in] evt The event to handle
      */
      void HandleMouseScrollEvent(const InputMouseScrollEvent* evt);

      /**
      * @brief Handles a joystick connect event by sending it to the
      *        correct joystick based on evt->id
      *
      * @param[in] evt The event to handle
      */
      void HandleJoystickConnectEvent(const InputJoystickConnectEvent* evt);

      /**
      * @brief Handles a joystick axis event by sending it to the
      *        correct joystick based on evt->id
      *
      * @param[in] evt The event to handle
      */
      void HandleJoystickAxisEvent(const InputJoystickAxisEvent* evt);

      /**
      * @brief Flushes all input filters
      */
      void Flush();

      /**
      * @brief Checks if a keyboard key is down this frame
      *
      * @param[in] key The key to check
      *
      * @return Is it down this frame?
      */
      SCRIPT_FUNC() bool IsKeyDown(Keys key) const;

      /**
      * @brief Checks if a keyboard key was pressed this frame
      *
      * @param[in] key The key to check
      *
      * @return Was it pressed this frame?
      */
      SCRIPT_FUNC() bool IsKeyPressed(Keys key) const;

      /**
      * @brief Checks if a keyboard key was released this frame
      *
      * @param[in] key The key to check
      *
      * @return Was it released this frame?
      */
      SCRIPT_FUNC() bool IsKeyReleased(Keys key) const;

      /**
      * @brief Checks if a mouse button is down this frame
      *
      * @param[in] button The button to check
      *
      * @return Is it down this frame?
      */
      SCRIPT_FUNC() bool IsMouseButtonDown(MouseButtons button) const;

      /**
      * @brief Checks if a mouse button was pressed this frame
      *
      * @param[in] button The button to check
      *
      * @return Was it pressed this frame?
      */
      SCRIPT_FUNC() bool IsMouseButtonPressed(MouseButtons button) const;

      /**
      * @brief Checks if a mouse button was released this frame
      *
      * @param[in] button The button to check
      *
      * @return Was it released this frame?
      */
      SCRIPT_FUNC() bool IsMouseButtonReleased(MouseButtons button) const;

      /**
      * @brief Checks if a joystick button is down this frame
      *
      * @param[in] id The joystick ID to check for
      * @param[in] button The button to check
      *
      * @return Is it down this frame?
      */
      SCRIPT_FUNC() bool IsJoystickButtonDown(
        int id, 
        JoystickButtons button) const;

      /**
      * @brief Checks if a joystick button was pressed this frame
      *
      * @param[in] id The joystick ID to check for
      * @param[in] button The button to check
      *
      * @return Was it pressed this frame?
      */
      SCRIPT_FUNC() bool IsJoystickButtonPressed(
        int id,
        JoystickButtons button) const;

      /**
      * @brief Checks if a joystick button was released this frame
      *
      * @param[in] id The joystick ID to check for
      * @param[in] button The button to check
      *
      * @return Was it released this frame?
      */
      SCRIPT_FUNC() bool IsJoystickButtonReleased(
        int id,
        JoystickButtons button) const;

      /**
      * @return The current mouse position, in pixels, from 0,0 to w,h
      */
      SCRIPT_FUNC() glm::vec2 GetMousePosition() const;

      /**
      * @return The current mouse movement, in pixels, of this frame
      */
      SCRIPT_FUNC() glm::vec2 GetMouseMovement() const;

      /**
      * @return The current mouse scroll delta of this frame
      */
      SCRIPT_FUNC() int GetMouseScroll() const;

      /**
      * @brief Retrieves the axis value of a joystick axis
      *
      * @param[in] id The joystick ID to retrieve an axis from
      * @param[in] axis The axis to retrieve the value of
      *
      * @return The value in the -1.0 to 1.0 range
      */
      SCRIPT_FUNC() float GetJoystickAxis(int id, JoystickAxes axis) const;

    private:

      /**
      * @brief The registered input filters
      *
      * These filters are used to read input data from and set the appropriate
      * input states for per event
      */
      foundation::Vector<IInputFilter*> filters_;

      Keyboard keyboard_; //!< The keyboard input event handler
      Mouse mouse_; //!< The mouse input event handler
      Joystick joysticks_[kNumSupportedJoysticks];
    };
  }
}