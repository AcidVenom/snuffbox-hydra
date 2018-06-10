#pragma once

#include "tools/compilers/definitions/asset_types.h"
#include "tools/builder/threading/build_item.h"

#include <foundation/memory/memory.h>

#include <thread>

namespace snuffbox
{
  namespace compilers
  {
    class ICompiler;
  }

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
      * @brief Used to sync back the compiled data to the main thread
      *
      * @author Daniel Konings
      */
      struct Result
      {
        bool success; //!< Was the compilation a success?
        foundation::String error; //!< The error if compilation failed
        const uint8_t* buffer; //!< The compiled buffer
        size_t length; //!< The length of the buffer
      };

      /**
      * @brief Default constructor
      */
      BuildJob();

      /**
      * @brief Starts the compilation of a build item with this job's thread
      *
      * This function does not return the data, as this is ran in a 
      * multi-threaded context. Instead, the user should check if the build job 
      * has finished and then retrieve the data if there were no errors.
      *
      * @param[in] item The item to build
      *
      * @return Were we able to start the build?
      */
      bool Compile(const BuildItem& item);

      /**
      * @brief Syncs back the data after compilation
      *
      * This function will also join the build job's thread back to the main
      * thread. If the build job is still running, it silently fails and
      * returns a nullptr struct.
      *
      * @return The result of the compilation
      */
      const Result& SyncData();

      /**
      * @return Are we ready to accept new compilations?
      */
      bool ready() const;

      /**
      * @return Do we have new data that needs to be flushed to disk?
      */
      bool has_data() const;

      /**
      * @return The current build item
      */
      const BuildItem& current_item() const;

      /**
      * @see BuildJob::Join
      */
      ~BuildJob();

    protected:

      /**
      * @brief Joins back the job's thread to the main thread if possible
      */
      void Join();

      /**
      * @brief Creates an asset compiler by type
      *
      * @param[in] type The type of the compiler to create
      *
      * @return The created compiler, or nullptr if a compiler for the type
      *         does not exist
      */
      static foundation::SharedPtr<compilers::ICompiler> 
        CreateCompiler(compilers::AssetTypes type);

    private:

      bool ready_; //!< Are we ready to accept new compilations?
      bool has_data_; //!< Do we have data that needs to be flushed to disk?

      std::thread thread_; //!< The thread this build job runs on

      /**
      * @brief The current compiler of this build job
      */
      foundation::SharedPtr<compilers::ICompiler> compiler_;

      BuildItem current_item_; //!< The current build item
      Result result_; //!< The result of the build job
    };
  }
}