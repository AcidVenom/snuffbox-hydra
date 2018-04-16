#pragma once

namespace snuffbox
{
  namespace engine
  {
    /**
    * @brief Used to push input events to a queue, for it to be flushed at
    *        the end of the frame
    *
    * The queue is filled from an IInputFilter and used by the InputService
    * to set the current key states
    *
    * @author Daniel Konings
    */
    class InputQueue
    {

    };
  }
}