#include "foundation/linux/linux_directory_listener.h"
#include "foundation/io/directory.h"

#include <sys/inotify.h>
#include <unistd.h>

namespace snuffbox
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    const uint32_t LinuxDirectoryListener::kTimeout_ = 500;
    const size_t LinuxDirectoryListener::kEventSize_ = 4096;

    //--------------------------------------------------------------------------
    LinuxDirectoryListener::LinuxDirectoryListener() :
      path_(""),
      is_ok_(false),
      should_exit_(false),
      on_directory_changed_(nullptr),
      on_file_changed_(nullptr)
    {
    }

    //--------------------------------------------------------------------------
    void LinuxDirectoryListener::SetCallbacks(
      const OnDirectoryChanged& on_directory_changed,
      const OnDirectoryChanged& on_file_changed)
    {
      on_directory_changed_ = on_directory_changed;
      on_file_changed_ = on_file_changed;
    }

    //--------------------------------------------------------------------------
    void LinuxDirectoryListener::Listen(const Path& root)
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
        while (should_exit_ == false)
        {
          Vector<struct pollfd>& pfds = data_.pfds;
          int ret = poll(pfds.data(), pfds.size(), kTimeout_);
          if (ret < 0)
          {
              should_exit_ = true;
          }
          else if (ret > 0)
          {
            for (size_t i = 0; i < pfds.size(); ++i)
            {
              struct pollfd& pfd = pfds.at(i);
              if ((pfd.revents & POLLIN) == POLLIN)
              {
                HandleEvent(pfd, i);
                break;
              }
            }
          }
        }
      });
    }

    //--------------------------------------------------------------------------
    void LinuxDirectoryListener::Stop()
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
    bool LinuxDirectoryListener::is_ok() const
    {
      return is_ok_;
    }

    //--------------------------------------------------------------------------
    bool LinuxDirectoryListener::Initialize(const Path& root)
    {
      if (Directory::Exists(root) == false)
      {
        return false;
      }

      Function<bool(const Path&)> recurse = [&](const Path& current)
      {
        Directory dir(current);

        const Vector<Path>& children = dir.children();
        for (size_t i = 0; i < children.size(); ++i)
        {
          const Path& child = children.at(i);
          if (child.is_directory() == true)
          {
            if (recurse(child) == false)
            {
              return false;
            }
          }
        }

        return AddDirectory(current.ToString().c_str());
      };

      return recurse(root);
    }

    //--------------------------------------------------------------------------
    bool LinuxDirectoryListener::AddDirectory(const char* path)
    {
      struct pollfd pfd;
      pfd.fd = inotify_init();
      pfd.events = POLLIN;
      pfd.revents = 0;

      if (pfd.fd == -1)
      {
        return false;
      }

      int wd = inotify_add_watch(pfd.fd, path, IN_ALL_EVENTS);

      if (wd == -1)
      {
        close(pfd.fd);
        return false;
      }

      data_.pfds.push_back(pfd);
      data_.wds.push_back(wd);
      data_.paths.push_back(path);

      return true;
    }

    //--------------------------------------------------------------------------
    void LinuxDirectoryListener::HandleEvent(struct pollfd& pfd, size_t i)
    {
      uint8_t buffer[kEventSize_];
      int ret = read(pfd.fd, buffer, kEventSize_);

      pfd.revents = 0;
      pfd.events = POLLIN;

      if (ret < 0)
      {
        RemoveEvent(i);
        return;
      }

      struct inotify_event* evt =
          reinterpret_cast<struct inotify_event*>(buffer);

      uint32_t m = evt->mask;
      bool is_dir = (m & IN_ISDIR) > 0;

      auto DoCallback = [=](const OnDirectoryChanged& cb)
      {
        if (cb != nullptr)
        {
          cb(path_);
        }
      };

      if ((m & IN_DELETE_SELF) > 0)
      {
        RemoveEvent(i);
        DoCallback(on_directory_changed_);
      }
      else if (
        (m & IN_MODIFY) > 0 ||
        (m & IN_MOVE) > 0 ||
        (m & IN_CREATE) > 0)
      {
        if (is_dir == true)
        {
          Path p = data_.paths.at(i) / evt->name;

          if ((m & IN_CREATE) > 0)
          {
            AddDirectory(p.ToString().c_str());
          }
          else if ((m & IN_MOVED_FROM) > 0)
          {
            RemoveEvent(p);
          }

          DoCallback(on_directory_changed_);
          return;
        }

        DoCallback(on_file_changed_);
      }
    }

    //--------------------------------------------------------------------------
    void LinuxDirectoryListener::RemoveEvent(size_t i)
    {
      if (i >= data_.pfds.size())
      {
        return;
      }

      struct pollfd pfd = data_.pfds.at(i);
      int wd = data_.wds.at(i);
      int fd = pfd.fd;

      if (fd != -1)
      {
        if (wd != -1)
        {
          inotify_rm_watch(fd, wd);
        }

        close(fd);
      }

      data_.pfds.erase(data_.pfds.begin() + i);
      data_.wds.erase(data_.wds.begin() + i);
      data_.paths.erase(data_.paths.begin() + i);

      for (int i = static_cast<int>(data_.paths.size()) - 1; i >= 0; --i)
      {
        if (Directory::Exists(data_.paths.at(i)) == false)
        {
          RemoveEvent(i);
        }
      }
    }

    //--------------------------------------------------------------------------
    void LinuxDirectoryListener::RemoveEvent(const Path& path)
    {
      size_t idx = 0;
      bool found = false;

      for (size_t i = 0; i < data_.paths.size(); ++i)
      {
        if (data_.paths.at(i) == path)
        {
          found = true;
          idx = i;
          break;
        }
      }

      if (found == false)
      {
        return;
      }

      RemoveEvent(idx);
    }

    //--------------------------------------------------------------------------
    void LinuxDirectoryListener::Close()
    {
      for (int i = static_cast<int>(data_.pfds.size() - 1); i >= 0; --i)
      {
        RemoveEvent(i);
      }
    }

    //--------------------------------------------------------------------------
    LinuxDirectoryListener::~LinuxDirectoryListener()
    {
      Close();
    }
  }
}
