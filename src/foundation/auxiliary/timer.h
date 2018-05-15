#pragma once

#include "foundation/containers/string.h"
#include "foundation/definitions/time_units.h"

#include <chrono>

namespace snuffbox
{
  namespace foundation
  {
    /**
    * @brief A timer class to profile and to use for delta-timing
    *
    * @author Daniel Konings
    */
    class Timer
    {

    public:

      /**
      * @brief Creates a timer by name
      *
      * @param[in] name The name of the timer
      */
      Timer(const String& name);

      /**
      * @brief Start the timer, defining the initial time point
      */
      void Start();

      /**
      * @brief Stop the timer, resetting the current elapsed time
      */
      void Stop();

      /**
      * @brief Pauses the timer, not resetting the current elapsed time
      */
      void Pause();

      /**
      * @brief Retrieves the current elapsed time converted to a specified
      *        time unit
      *
      * @param[in] unit The unit to retrieve the elapsed time as
      * @param[in] print Should the result be printed to the console?
      *
      * @return The elapsed time in the specified unit
      */
      float Elapsed(
        TimeUnits unit = TimeUnits::kMillisecond,
        bool print = false);

      /**
      * @return The name of this timer
      */
      const String& name() const;

    protected:

      /**
      * @brief Updates the elapsed time
      */
      void Update();

      /**
      * @brief Moves the current time point the the 'now' time point
      */
      void MoveCurrent();

    private:

      String name_; //!< The name of this timer
      bool paused_; //!< Is the timer paused?
      bool stopped_; //!< Have we completely stopped the timer?

      float elapsed_; //!< The currently elapsed time in milliseconds
      float prev_elapsed_; //!< The previous elapsed time before pause/stop

      /**
      * @brief The current time point to check the elapsed time against
      */
      std::chrono::high_resolution_clock::time_point current_;
    };
  }
}