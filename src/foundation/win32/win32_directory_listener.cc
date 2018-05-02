#include "foundation/win32/win32_directory_listener.h"

namespace snuffbox
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    const DWORD Win32DirectoryListener::kTimeout_ = 500;

    //--------------------------------------------------------------------------
    Win32DirectoryListener::Win32DirectoryListener() :
      path_(""),
      is_ok_(false),
      should_exit_(false),
      on_directory_changed_(nullptr),
      on_file_changed_(nullptr),
      refresh_handle_(NULL),
      file_handle_(NULL)
    {

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
    void Win32DirectoryListener::Listen(const Path& root)
    {
      path_ = root;
      is_ok_ = Initialize(path_);

      if (is_ok_ == false)
      {
        return;
      }

      should_exit_ = false;

      thread_ = std::thread([&]()
      {
        DWORD wait_status;
        HANDLE handles[] =
        {
          refresh_handle_,
          file_handle_
        };

        DWORD count = static_cast<DWORD>(sizeof(handles) / sizeof(HANDLE));

        OnDirectoryChanged* callbacks[] =
        {
          &on_directory_changed_,
          &on_file_changed_
        };

        while (should_exit_ == false)
        {
          wait_status = WaitForMultipleObjects(
            count,
            handles, 
            FALSE, 
            kTimeout_); 

          for (DWORD i = 0; i < count; ++i)
          {
            if (wait_status == WAIT_OBJECT_0 + i)
            {
              OnDirectoryChanged& cb = *callbacks[i];
              if (cb != nullptr)
              {
                cb(path_);
              }

              if (FindNextChangeNotification(handles[i]) == FALSE)
              {
                return;
              }

              break;
            }
          }
        }
      });
    }

    //--------------------------------------------------------------------------
    void Win32DirectoryListener::Stop()
    {
      if (is_ok_ == false)
      {
        return;
      }

      should_exit_ = true;

      if (thread_.joinable() == true)
      {
        thread_.join();
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