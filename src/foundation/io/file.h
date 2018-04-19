#pragma once

#include "foundation/io/path.h"
#include "foundation/definitions/io.h"

#include <fstream>

namespace snuffbox
{
  namespace foundation
  {
    /**
    * @brief A file class to open files with and read data from them
    *
    * File data can be accessed from both the system's file system and the
    * virtual file system in Snuffbox, to retrieve data compiled into the
    * exectuable.
    *
    * Files are opened at their construction and closed at their destruction.
    * If files are supposed to stay opened, keep this in mind.
    *
    * @author Daniel Konings
    */
    class File
    {

    public:

      /**
      * @brief A type definition for the open modes, which is simply an int32_t
      */
      using FileOpenMode = int32_t;

      /**
      * @brief Default constructor, creates an invalid file
      */
      File();

      /**
      * @brief Opens a file by file path
      *
      * Opening a file doesn't guarantee it to be opened, File::is_ok should
      * be checked before executing functionalities with the file.
      *
      * @param[in] path The relative, full or virtual path to the file
      * @param[in] mode The opening mode to use for this file
      *
      * @see FileFlags
      */
      File(const Path& path, FileOpenMode mode = FileFlags::kRead);

      /**
      * @brief Reads the file's buffer into memory
      *
      * @param[out] length The length of the file
      * @param[in] is_string Should a null-terminator character be appended?
      *
      * @return The file contents as binary data
      */
      uint8_t* ReadBuffer(size_t* length, bool is_string = false);

      /**
      * @return Is the file open and able to be used?
      */
      bool is_ok() const;

      /**
      * @return The file length, excluding null-terminator 
      */
      size_t length() const;

      /**
      * @brief Destructor, closes the underlying stream if it was opened
      */
      ~File();

    protected:

      /**
      * @brief Converts Snuffbox's file flags to an std::ios::openmode
      *
      * @param[in] flags The flags to convert
      *
      * @return The converted openmode
      */
      static std::ios::openmode FileFlagsToOpenMode(FileFlags flags);

    private:

      std::fstream stream_; //!< The file stream for this file
      bool is_ok_; //!< Is the file open and able to be used?

      uint8_t* buffer_; //!< The buffer of this file
      size_t length_; //!< The file length
    };
  }
}