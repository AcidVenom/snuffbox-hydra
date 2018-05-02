#pragma once

#include "foundation/definitions/io.h"
#include "foundation/io/path.h"

#include <thread>
#include <poll.h>

namespace snuffbox
{
  namespace foundation
  {
    /**
    * @brief Listens to directories for either file or directory changes,
    *        using inotify on Linux based systems
    *
    * The changes that are listened for include renames, file modifications,
    * deletions and additions.
    *
    * The listener should be used thread safe so that it can run on
    * a different thread and not block the main thread.
    *
    * The listener only tells the user the root directory or its subdirectories
    * have changed. There is however a distinction between tree changes and
    * file changes, for which there are different callbacks. The listener
    * doesn't specifically list which file or directory has changed.
    *
    * @author Daniel Konings
    */
    class LinuxDirectoryListener
    {

    protected:

      /**
      * @brief The data used by the directory listener to remove and add
      *        watches accordingly using inotify
      *
      * @remarks This structure is used as a struct of arrays for a better
      *          memory access pattern
      */
      struct PollData
      {
        Vector<struct pollfd> pfds; //!< The polling data used by inotify
        Vector<int> wds; //!< The watch descriptors created from the inotify
        Vector<Path> paths; //!< The paths that are being watched currently
      };

    public:

      /**
      * @brief Creates an invalid listener for later use
      */
      LinuxDirectoryListener();

      /**
      * @brief Sets the callback that is called when a directory or its contents
      *        have changed and the callback for when a file changed
      *
      * It is not thread-safe to set these callbacks during listening
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
      * @param[in] root The directory to start listening on
      *
      * @see LinuxDirectoryListener::Initialize
      *
      * This call is non-blocking and ran on a different thread
      */
      void Listen(const Path& root);

      /**
      * @brief Notifies the directory listener to exit, joining the listening
      *        thread back to the main thread
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

      /**
      * @brief Adds a directory for watching by path
      *
      * @param[in] path The path to the existing directory
      *
      * @return Was both the file and watch descriptor initialized properly?
      */
      bool AddDirectory(const char* path);

      /**
      * @brief Handles a poll event for a file descriptor
      *
      * @param[in] pfd The poll event file descriptor to handle
      * @param[in] i The index of the event in the
      *              LinuxDirectoryListener::pfds_ array
      */
      void HandleEvent(struct pollfd& pfd, size_t i);

      /**
      * @brief Removes an event at a provided index
      *
      * @param[in] i The index of the event to remove
      *
      * @remarks This function verifies that underlying paths still exist,
      *          if they do not, they're removed as well.
      */
      void RemoveEvent(size_t i);

      /**
      * @brief Removes an event by path instead of index
      *
      * @param[in] path The path to a registered event
      *
      * @see LinuxDirectoryListener::RemoveEvent
      */
      void RemoveEvent(const Path& path);

      /**
      * @brief Closes the file descriptors, also when an error has occured
      *        during initialization
      *
      * @remarks This function checks if the file descriptors were opened
      *          first, before closing them
      */
      void Close();

    public:

      /**
      * @brief Closes the inotify file descriptors if they were initialized
      *
      * @see LinuxDirectoryListener::Close
      */
      ~LinuxDirectoryListener();

    private:

      Path path_; //!< The root path of the listener
      bool is_ok_; //!< Is the directory listener available for use?
      bool should_exit_; //!< Should the directory listener stop listening?

      /**
      * @brief The different poll file descriptors per directory
      */
      PollData data_;

      /**
      * @brief The callback when a directory or its contents have changed
      */
      OnDirectoryChanged on_directory_changed_;

      /**
      * @brief The callback when a file's contents have changed
      */
      OnDirectoryChanged on_file_changed_;

      std::thread thread_; //!< The thread the listener runs on

      /**
      * @brief The timeout of the listener in milliseconds
      *
      * @remarks After this period the listener checks again if it should exit
      *          and if not; continues the listening events
      */
      static const uint32_t kTimeout_;

      /**
      * @brief The event size of an inotify_event, including the name
      */
      static const size_t kEventSize_;
    };
  }
}
