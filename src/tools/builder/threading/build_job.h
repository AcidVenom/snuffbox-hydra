#pragma once

#include <thread>

namespace snuffbox
{
  namespace builder
  {
    /**
    * @brief A build job to compile or decompile a file with
    *
    * This job runs multi-threaded and notifies the build scheduler when it
    * has finished building the file. The data can then be retrieved from
    * this build job itself.
    *
    * @author Daniel Konings
    */
    class BuildJob
    {

    public:

      /**
      * @return Is this build job ready to build a new file?
      */
      bool IsReady() const;

    private:

      std::thread thread_; //!< The thread this build job runs on
    };
  }
}