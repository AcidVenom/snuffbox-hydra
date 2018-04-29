#pragma once

#include "tools/builder/threading/build_job.h"

#include <foundation/io/path.h>
#include <foundation/containers/vector.h>

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

    private:

      foundation::Vector<BuildJob> jobs_; //!< The build jobs that exist
    };
  }
}