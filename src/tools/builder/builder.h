#pragma once

#include "tools/builder/threading/build_scheduler.h"

#include <foundation/io/path.h>
#include <foundation/auxiliary/logger.h>

#include <foundation/io/directory_tree.h>
#include <foundation/io/directory_listener.h>

namespace snuffbox
{
  namespace builder
  {
    /**
    * @brief The builder to compile all assets into a format readable by
    *        the engine
    *
    * Assets have custom compilers to build files based on their file extension.
    * These files can then be decompiled with the same compilers by the engine
    * itself. This makes for a workflow where the builder builds a file, outputs
    * it to a build directory and rebuilds it accordingly when the source file
    * has changed.
    *
    * The builder will retain the same folder structure as the original source
    * directory's tree.
    *
    * This is a multi-threaded process, where every core of the CPU can build
    * its own file. There is no job system attached, as it's simply input ->
    * output. There should be no data contentation.
    *
    * @see BuildScheduler
    * @see BuildJob
    *
    * @author Daniel Konings
    */
    class Builder
    {

    protected:

      /**
      * @brief A structure to keep track of when files were changed so they
      *        can be rebuilt automatically
      *
      * @author Daniel Konings
      */
      struct FileTime
      {
        time_t last_modified; //!< When was the file last modified?
      };

    public:

      /**
      * @brief A short hand for DirectoryTreeItem listings
      */
      using ItemList = foundation::Vector<foundation::DirectoryTreeItem>;

      /**
      * @brief Creates an invalid builder
      */
      Builder();

      /**
      * @brief Initializes the builder with a source directory and
      *        build directory
      *
      * The directory tree of the source directory is created here and compared
      * against the build directory, if there is a mismatch it is reconstructed.
      *
      * The build directory will be created at the source directory's level
      * under ./build. The source directory will be scanned from the top level
      * for files.
      *
      * @remarks This method fails if the source directory is invalid or
      *          the build directory could not be created
      *
      * @see BuildScheduler::Refresh
      *
      * @return Was the initialization succesful?
      */
      bool Initialize(const foundation::Path& source_dir);

      /**
      * @brief Notify the builder that the application is idle and that the
      *        build scheduler can start doing work
      *
      * @see BuildScheduler::IdleNotification
      */
      void IdleNotification();

      /**
      * @brief Shuts down the builder and its directory listener
      *
      * @remarks This function must be called before the exit of the application
      *          as the listener thread is joined here, along with the
      *          BuildScheduler
      */
      void Shutdown();

      /**
      * @return Is the builder ready for use?
      */
      bool is_ok() const;

    protected:

      /**
      * @brief Creates the build directory under the source-directory/build
      *
      * @remarks This function gets called again during Builder::SyncDirectories
      *          in case the user deleted the build directory
      *
      * @return Were we able to create the build directory?
      */
      bool CreateBuildDirectory() const;

      /**
      * @brief Syncs the build directory's directory structure with the
      *        structure of the source directory
      */
      void SyncDirectories();
      
      /**
      * @brief Finds the file changes when either a file has changed or the
      *        builder is started up initially.
      *
      * The file changes are tracked using .time files, which contain data on
      * when the file was last built. If the file does not exist, it is created
      * alongside of the file and the file is marked for build.
      *
      * @param[in] items The current item list we're traversing for file changes
      */
      void FindFileChanges(const ItemList& items);

      /**
      * @brief Syncs the items from the source tree structure to the build
      *        tree
      *
      * @param[in] source_items The list of source items
      */
      void SyncItems(const ItemList& source_items) const;

      /**
      * @brief Removes folders that are not in the source tree anymore
      *
      * @param[in] build_items The list of build items
      */
      void RemoveOld(const ItemList& build_items) const;

      /**
      * @brief Finds the time stamp of a file and if it does not exist, it
      *        is created. It is then checked for rebuild.
      *
      * The timestamp is created alongside of the file in the build directory,
      * next to the relative path of the file in the source directory.
      *
      * If the timestamp already exists, it is compared against the source file
      * to see if their modification times match. If not, the file can be
      * marked for rebuild.
      *
      * @param[in] path The path to the file to check, in the source directory
      *
      * @return Should the file be rebuilt?
      */
      bool HasChanged(const foundation::Path& path) const;

      /**
      * @brief Queues a file for build by path
      *
      * @param[in] path The path to the source file
      */
      void QueueForBuild(const foundation::Path& path);

    public:

      /**
      * @brief Stops the builder if it was running
      *
      * @see Builder::Shutdown
      */
      ~Builder();

    private:

      bool is_ok_; //!< Is the builder ready for use?

      foundation::DirectoryTree source_tree_; //!< The source directory tree
      foundation::DirectoryTree build_tree_; //!< The build directory tree

      foundation::Path source_directory_; //!< The current source directory
      foundation::Path build_directory_; //!< The current build directory

      foundation::DirectoryListener listener_; //!< The directory listener

      BuildScheduler scheduler_; //!< The build scheduler to queue files in

      /**
      * @brief The name of the build directory folder
      */
      static const char* kBuildFolder_;

      /**
      * @brief The extension for the time stamp files
      */
      static const char* kStampExtension_;
    };
  }
}
