#pragma once

#include "engine/input/input_event_handler.h"
#include "engine/definitions/keycodes.h"

#include <cstddef>

namespace snuffbox
{
  namespace engine
  {
    struct InputMouseMoveEvent;
    struct InputMouseScrollEvent;

    /**
    * @brief Used to handle mouse events, like mouse move, scroll and mouse
    *        button presses
    *
    * @author Daniel Konings
    */
    class Mouse : public IInputEventHandler
    {

    public:

      /**
      * @brief Default constructor, changes all mouse button states to
      *        KeyButtonState::kUp and sets current mouse deltas to 0
      */
      Mouse();

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
      * @see InputService::HandleMouseMoveEvent
      */
      void HandleMouseMoveEvent(const InputMouseMoveEvent* evt);

      /**
      * @see InputService::HandleMouseScrollEvent
      */
      void HandleMouseScrollEvent(const InputMouseScrollEvent* evt);

      /**
      * @brief Retrieves the mouse position
      *
      * @param[out] x The current horizontal mouse position in pixels
      * @param[out] y The current vertical mouse position in pixels
      */
      void GetMousePosition(int* x, int* y) const;

      /**
      * @brief Retrieves the mouse movement
      *
      * @param[out] dx The current movement on the horizontal axis, in pixels
      * @param[out] dy The current movement on the vertical axis, in pixels
      */
      void GetMouseMovement(int* dx, int* dy) const;

      /**
      * @return The current scroll factor of this frame
      */
      int scroll() const;

    private:

      int x_; //!< The current horizontal mouse position in pixels
      int y_; //!< The current vertical mouse position in pixels

      int dx_; //!< The current delta movement on the horizontal axis, in pixels
      int dy_; //!< The current delta movement on the vertical axis, in pixels

      int scroll_; //!< The current scroll factor of this frame

      /**
      * @brief The current mouse button states
      */
      KeyButtonState states_[static_cast<size_t>(MouseButtons::kCount)];
    };
  }
}