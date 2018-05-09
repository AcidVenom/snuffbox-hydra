#pragma once

#include "tools/builder/threading/build_job.h"
#include "tools/builder/threading/build_item.h"

#include <foundation/containers/vector.h>

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

      /**
      * @brief Joins the build jobs back to the main thread by destructing them
      */
      ~BuildScheduler();

    protected:

      /**
      * @brief Flushes the queue of build items to the build jobs
      */
      void Flush();

      /**
      * @brief Attempts to schedule a compilation for the build jobs
      *
      * If no build job is currently available as they're already building,
      * we wait until our next possible moment to recompile.
      *
      * @param[in] item The item to schedule
      * @param[out] id The job that will build the file, if found
      *
      * @return Were we able to schedule the item for build?
      */
      bool ScheduleJob(const BuildItem& item, size_t* id);
      
    private:

      foundation::Vector<BuildJob*> jobs_; //!< The build jobs that exist
      BuildQueue queue_; //!< The queue of build items, ordered by asset type

      /**
      * @brief Used to safely queue items in the build queue from a different
      *        thread
      */
      std::mutex queue_mutex_;
    };
  }
}