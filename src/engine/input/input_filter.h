#pragma once

#include <cstddef>

namespace snuffbox
{
  namespace engine
  {
    struct InputEvent;

    /**
    * @brief Used as an interface for all input filters, to filter native input
    *        events and cast them to the appropriate Snuffbox keycodes
    *
    * @author Daniel Konings
    */
    class IInputFilter
    {

    public:

      /**
      * @brief Construct with a buffer
      *
      * @param[in] max_buffered_events The maximum number of input events that
      *                                can be buffered
      */
      IInputFilter(size_t max_buffered_events);

      /**
      * @brief Retrieves the next input event in the buffer, from the back
      *
      * @param[out] e The found input event, or nullptr if at the end
      *
      * @remarks This pops off the last event (if any) in the buffer
      *
      * @return Are there still input events left?
      */
      bool GetNext(const InputEvent** e);

    protected:

      /**
      * @brief Pushes a new input event into the buffer
      *
      * This copies the event's data into the current buffer, from the current
      * buffer index
      *
      * @param[in] e The event to push
      */
      void BufferEvent(const InputEvent* e);

    public:

      /**
      * @brief Deallocates the buffer if it is allocated
      */
      virtual ~IInputFilter();

    private:

      void* buffer_; //!< The input buffer
      size_t buffer_size_; //!< The buffer size
      size_t num_events_; //!< The number of current events

      /**
      * @brief The event size to allocate into the buffer, this should be
      *        large enough to contain the data of every possible event
      *
      * @see InputEvent and its derived members
      */
      static const size_t kEventSize_;
    };
  }
}