#pragma once

#include "foundation/io/path.h"
#include "foundation/containers/vector.h"

namespace snuffbox
{
  namespace foundation
  {
    /**
    * @brief A class to wrap native Win32 directory modifications
    *
    * As this class is simply typedef'd to "Directory", its interface should
    * match the ones of different platforms (e.g. LinuxDirectory)
    *
    * @author Daniel Konings
    */
    class Win32Directory
    {

    public:

      /**
      * @brief Creates an invalid directory
      */
      Win32Directory();

      /**
      * @see Win32Directory::Open
      */
      Win32Directory(const Path& path);

      /**
      * @brief Opens a directory from a provided path
      *
      * This call will create the directory if it does not exist yet
      *
      * @see Win32Directory::Exists
      *
      * @param[in] path The path to the directory
      */
      bool Open(const Path& path);

      /**
      * @brief Checks if a directory exists
      *
      * If the directory is either a virtual path or not a directory, this
      * call will fail.
      *
      * @param[in] path The path to the directory
      *
      * @return Was the path a directory?
      */
      static bool Exists(const Path& path);

      /**
      * @brief Removes a directory from a provided path
      *
      * This call will fail if the directory does not exist, but will fail
      * silently.
      *
      * @see Win32Directory::Exists
      *
      * @param[in] path The path to the directory to remove
      */
      static void Remove(const Path& path);

      /**
      * @brief Is this directory valid for use?
      */
      bool is_ok() const;

      /**
      * @return The children of this directory
      */
      const Vector<Path>& children() const;

    protected:

      /**
      * @brief Attempts to create a directory from a provided path using
      *        CreateDirectoryA
      *
      * The directory path is first split up into its seperate directories
      * and checks if those directories exist first. If these don't exist,
      * they get created as well.
      *
      * @param[in] path The path to the directory to create
      *
      * @return Was the creation of the directory a success?
      */
      static bool CreateDirectory(const Path& path);

      /**
      * @brief Enumerates the children from a provided directory
      *
      * This call will fail if the directory does not exist
      *
      * @see Win32Directory::Exists
      *
      * @param[in] path The path to the directory to enumerate
      *
      * @return The list of paths to the children of this directory
      */
      static Vector<Path> EnumerateChildren(const Path& path);

    private:

      bool is_ok_; //!< Is this directory valid for use?
      Vector<Path> children_; //!< The children of this directory
    };
  }
}