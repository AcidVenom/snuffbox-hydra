#pragma once

#include "foundation/definitions/io.h"
#include "foundation/io/path.h"

#include "foundation/win32/win32_include.h"

#include <thread>

namespace snuffbox
{
  namespace foundation
  {
    /**
    * @brief Listens to directories for either file or directory changes,
    *        using FindFirstChangeNotification in the Win32 API
    *
    * The changes that are listened for include renames, file modifications,
    * deletions and additions.
    *
    * The listener should be designed thread safe so that it can run on
    * a different thread and not block the main thread.
    *
    * @author Daniel Konings
    */
    class Win32DirectoryListener
    {

    public:

      /**
      * @brief Initializes the directory listener on a specified root folder
      *
      * @see Win32DirectoryListener::Initialize
      */
      Win32DirectoryListener(const Path& root);

      /**
      * @brief Sets the callback that is called when a directory or its contents
      *        have changed and the callback for when a file changed
      *
      * @param[in] on_directory_changed When a directory changed
      * @param[in] on_file_changed When a file changed
      */
      void SetCallbacks(
        const OnDirectoryChanged& on_directory_changed,
        const OnDirectoryChanged& on_file_changed);

      /**
      * @brief This function will start listening for directory changes
      *
      * This call is non-blocking and ran on a different thread
      */
      void Listen();

      /**
      * @brief Notifies the directory listener to exit, joining the thread back
      *        to the main thread
      */
      void Stop();

      /**
      * @return Is the directory listener available for use?
      */
      bool is_ok() const;

    protected:

      /**
      * @brief Initializes the directory listener by setting up the change
      *        handles on the root directory
      *
      * @param[in] root The root directory to listen on
      */
      bool Initialize(const Path& root);

    public:

      /**
      * @brief Closes the change handles if they were initialized
      */
      ~Win32DirectoryListener();

    private:

      Path path_; //!< The root path of the listener
      bool is_ok_; //!< Is the directory listener available for use?
      bool should_exit_; //!< Should the directory listener stop listening?

      /**
      * @brief The callback when a directory or its contents have changed
      */
      OnDirectoryChanged on_directory_changed_;

      /**
      * @brief The callback when a file's contents have changed
      */
      OnDirectoryChanged on_file_changed_;

      HANDLE refresh_handle_; //!< The change handle to track tree changes
      HANDLE file_handle_; //!< The change handle to track file changes

      std::thread thread_; //!< The thread the listener runs on

      /**
      * @brief The timeout if the listener in milliseconds
      *
      * @remarks After this period the listener checks again if it should exit
      *          and if not; continues the listening events
      */
      static const DWORD kTimeout_;
    };
  }
}