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
      * @see File::Open
      */
      File(const Path& path, FileOpenMode mode = FileFlags::kRead);

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
      bool Open(const Path& path, FileOpenMode mode = FileFlags::kRead);

      /**
      * @brief Closes the file
      */
      void Close();

      /**
      * @brief Reads the file's buffer into memory
      *
      * The buffer is only read into memory if the file is non-virtual, if
      * the file is virtual there's a direct mapping to the buffer in memory.
      * The buffer should not be deleted by the user, the buffer's lifetime
      * is either managed by the file for non-virtual data, or by the resource
      * system for virtual data.
      *
      * @param[out] length The length of the file
      * @param[in] is_string Should a null-terminator character be appended?
      *
      * @remarks is_string is currently only applicable for non-virtual files.
      *          When length is nullptr, the function will silently fail.
      *
      * @return The file contents as binary data
      */
      const uint8_t* ReadBuffer(size_t* length, bool is_string = false);

      /**
      * @brief Writes data to an opened file
      *
      * @param[in] buffer The buffer to write
      * @param[in] size The size of the buffer
      */
      void Write(const uint8_t* buffer, size_t size);

      /**
      * @brief Removes a file silently
      *
      * @remarks This call can fail, but will not notify the user
      *
      * @param[in] path The path to the file to remove
      */
      static void Remove(const Path& path);

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
      *
      * @see File::Close
      */
      ~File();

    protected:

      /**
      * @brief Used to open a 'file' from the virtual resources system
      *
      * @see Resources
      *
      * @param[in] path The path to the virtual resource with the virtual
      *                 prefix
      *
      * @return Was the 'file' loaded correctly?
      */
      bool OpenVirtual(const Path& path);

      /**
      * @brief Used to open a file from the system's file system
      *
      * @param[in] path The path to the file to open
      * @param[in] mode The open mode for the file
      *
      * @return Was the file loaded correctly?
      */
      bool OpenFile(const Path& path, FileOpenMode mode);

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

      const uint8_t* virtual_buffer_; //!< The virtual buffer of this file
      uint8_t* buffer_; //!< The buffer of this file
      size_t length_; //!< The file length
    };
  }
}