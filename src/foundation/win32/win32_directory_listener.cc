#include "foundation/win32/win32_directory_listener.h"

namespace snuffbox
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    Win32DirectoryListener::Win32DirectoryListener(const Path& root) :
      path_(root.ToString()),
      is_ok_(false),
      on_directory_changed_(nullptr),
      on_file_changed_(nullptr),
      refresh_handle_(NULL),
      file_handle_(NULL)
    {
      is_ok_ = Initialize(root);
    }

    //--------------------------------------------------------------------------
    void Win32DirectoryListener::SetCallbacks(
      const OnDirectoryChanged& on_directory_changed,
      const OnDirectoryChanged& on_file_changed)
    {
      on_directory_changed_ = on_directory_changed;
      on_file_changed_ = on_file_changed;
    }

    //--------------------------------------------------------------------------
    void Win32DirectoryListener::Listen()
    {
      if (is_ok_ == false)
      {
        return;
      }

      DWORD wait_status;
      HANDLE handles[] =
      {
        refresh_handle_,
        file_handle_
      };

      while (true)
      {
        wait_status = WaitForMultipleObjects(
          2,
          handles, 
          FALSE, 
          INFINITE); 

        switch (wait_status)
        {

        case WAIT_OBJECT_0:

          if (on_directory_changed_ != nullptr)
          {
            on_directory_changed_(path_);
          }

          if (FindNextChangeNotification(handles[0]) == FALSE)
          {
            return;
          }

          break;

        case WAIT_OBJECT_0 + 1:

          if (on_file_changed_ != nullptr)
          {
            on_file_changed_(path_);
          }

          if (FindNextChangeNotification(handles[1]) == FALSE)
          {
            return;
          }

          break;
        }
      }
    }

    //--------------------------------------------------------------------------
    bool Win32DirectoryListener::is_ok() const
    {
      return is_ok_;
    }

    //--------------------------------------------------------------------------
    bool Win32DirectoryListener::Initialize(const Path& root)
    {
      const char* path = path_.ToString().c_str();

      refresh_handle_ = FindFirstChangeNotificationA(
        path,
        TRUE,
        FILE_NOTIFY_CHANGE_CREATION | 
        FILE_NOTIFY_CHANGE_DIR_NAME | 
        FILE_NOTIFY_CHANGE_FILE_NAME);

      if (refresh_handle_ == NULL)
      {
        return false;
      }

      file_handle_ = FindFirstChangeNotificationA(
        path,
        TRUE,
        FILE_NOTIFY_CHANGE_LAST_WRITE);

      if (file_handle_ == NULL)
      {
        return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------
    Win32DirectoryListener::~Win32DirectoryListener()
    {
      if (refresh_handle_ != NULL)
      {
        FindCloseChangeNotification(refresh_handle_);
        refresh_handle_ = NULL;
      }

      if (file_handle_ != NULL)
      {
        FindCloseChangeNotification(file_handle_);
        file_handle_ = NULL;
      }
    }
  }
}