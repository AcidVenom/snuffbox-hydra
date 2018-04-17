#pragma once

#include "engine/services/service.h"

#include "engine/input/keyboard.h"
#include "engine/input/mouse.h"

#include <foundation/containers/vector.h>

namespace snuffbox
{
  namespace engine
  {
    class IInputFilter;

    struct InputKeyboardKeyEvent;
    struct InputMouseButtonEvent;
    struct InputMouseMoveEvent;

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
    class InputService : public ServiceBase<InputService>
    {

    public:

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
      * @brief Handles a mouse move event by sending it to
      *        InputService::mouse_
      *
      * @param[in] evt The event to handle
      */
      void HandleMouseMoveEvent(const InputMouseMoveEvent* evt);

      /**
      * @brief Flushes all input filters
      */
      void Flush();

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
    };
  }
}