#pragma once

#include "tools/builder/threading/build_job.h"
#include "tools/builder/threading/build_item.h"

#include <foundation/containers/vector.h>

#include <mutex>

namespace snuffbox
{
  namespace builder
  {
    class Builder;

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
      *
      * @param[in] builder The builder that owns this build scheduler
      */
      void IdleNotification(Builder* builder);

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
      * @brief Writes any compiled data to disk using the builder
      *
      * @param[in] builder The builder that owns this build scheduler
      */
      void WriteCompiled(Builder* builder);

      /**
      * @brief Attempts to schedule a compilation for the build jobs
      *
      * If no build job is currently available as they're already building,
      * we wait until our next possible moment to recompile.
      *
      * @param[in] item The item to schedule
      *
      * @return Were we able to schedule the item for build?
      */
      bool ScheduleJob(const BuildItem& item);

      /**
      * @brief Updates the build status and checks whether we finished building
      *        yet
      */
      void UpdateBuildStatus();

      /**
      * @return Are we currently building?
      */
      bool IsBuilding() const;

      /**
      * @brief Stops all build jobs, but waits for them to finish their current
      *        compilation
      *
      * In the BuildJob itself, the thread is joined from the build thread
      * to the main thread in its destructor.
      */
      void Stop();
      
    private:

      foundation::Vector<BuildJob*> jobs_; //!< The build jobs that exist
      BuildQueue queue_; //!< The queue of build items, ordered by asset type

      bool was_building_; //!< Were we building before queuing an item?
      uint32_t build_count_; //!< The number of items to build currently

      /**
      * @brief Used to safely queue items in the build queue from a different
      *        thread
      */
      std::mutex queue_mutex_;
    };
  }
}