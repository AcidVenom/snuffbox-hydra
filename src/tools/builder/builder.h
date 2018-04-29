#pragma once

#include <foundation/io/path.h>
#include <foundation/auxiliary/logger.h>

#include <foundation/io/directory_tree.h>

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
      * @return Is the builder ready for use?
      */
      bool is_ok() const;

    public:

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
      * @brief Syncs the items from the source tree structure to the build
      *        tree
      *
      * @param[in] source_items The list of source items
      */
      void SyncItems(const ItemList& source_items) const;

      /**
      * @brief Removes folders that are not in the source tree anymore
      *
      *
      * @param[in] build_items The list of build items
      */
      void RemoveOld(const ItemList& build_items) const;

    private:

      bool is_ok_; //!< Is the builder ready for use?

      foundation::DirectoryTree source_tree_; //!< The source directory tree
      foundation::DirectoryTree build_tree_; //!< The build directory tree

      foundation::Path source_directory_; //!< The current source directory
      foundation::Path build_directory_; //!< The current build directory
    };
  }
}