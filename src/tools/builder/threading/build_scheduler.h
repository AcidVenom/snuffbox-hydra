#pragma once

#include "tools/builder/threading/build_job.h"

#include <foundation/io/path.h>
#include <foundation/containers/vector.h>
#include <foundation/containers/queue.h>

#include <mutex>

namespace snuffbox
{
  namespace builder
  {
    /**
    * @brief Schedules build jobs to compile a file and output it to the
    *        build directory, multi-threaded
    *
    * @see BuildJob
    *
    * @author Daniel Konings
    */
    class BuildScheduler
    {

    public:

      /**
      * @brief Used to queue build items and schedule them to the corresponding
      *        build jobs
      *
      * @author Daniel Konings
      */
      struct BuildItem
      {
        foundation::Path in; //!< The path to the input source file
        foundation::Path out; //!< The path to the output build file
      };

      /**
      * @brief Default constructor
      *
      * @remarks Creates an invalid build scheduler
      */
      BuildScheduler();

      /**
      * @brief Queues a file for build by specifying its input and output path
      *
      * @param[in] item The build item to queue
      */
      void Queue(const BuildItem& item);

      /**
      * @brief Notifies the builder that the application is currently idle
      *        and can flush the queue of build items
      */
      void IdleNotification();

    protected:

      /**
      * @brief Flushes the queue of build items to the build jobs
      */
      void Flush();

    private:

      foundation::Vector<BuildJob> jobs_; //!< The build jobs that exist
      foundation::Queue<BuildItem> queue_; //!< The queue of build items

      /**
      * @brief Used to safely queue items in the build queue from a different
      *        thread
      */
      std::mutex queue_mutex_;
    };
  }
}