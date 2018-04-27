#include "foundation/linux/linux_directory.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

namespace snuffbox
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    LinuxDirectory::LinuxDirectory() :
      is_ok_(false)
    {

    }

    //--------------------------------------------------------------------------
    LinuxDirectory::LinuxDirectory(const Path& path) :
      is_ok_(false)
    {
      Open(path);
    }

    //--------------------------------------------------------------------------
    bool LinuxDirectory::Open(const Path& path)
    {
      if (path.is_virtual() == true)
      {
        return false;
      }

      if (Exists(path) == false)
      {
        is_ok_ = CreateDirectory(path);

        if (is_ok_ == false)
        {
          return false;
        }
      }

      children_ = EnumerateChildren(path);
      is_ok_ = true;

      return true;
    }

    //--------------------------------------------------------------------------
    bool LinuxDirectory::Exists(const Path& path)
    {
      DIR* dp = opendir(path.ToString().c_str());
      bool exists = dp != nullptr;

      if (exists == true)
      {
        closedir(dp);
      }

      return exists;
    }

    //--------------------------------------------------------------------------
    void LinuxDirectory::Remove(const Path& path)
    {
      struct stat st;
      const char* start_at = path.ToString().c_str();
      const char* child_path;

      LinuxDirectory dir(path);

      const Vector<Path>& children = dir.children();

      for (size_t i = 0; i < children.size(); ++i)
      {
        const Path& child = children.at(i);
        child_path = child.ToString().c_str();

        stat(child_path, &st);

        if (S_ISREG(st.st_mode) > 0)
        {
          unlink(child_path);
          continue;
        }

        Remove(child_path);
      }

      rmdir(start_at);
    }

    //--------------------------------------------------------------------------
    bool LinuxDirectory::is_ok() const
    {
      return is_ok_;
    }

    //--------------------------------------------------------------------------
    const Vector<Path>& LinuxDirectory::children() const
    {
      return children_;
    }

    //--------------------------------------------------------------------------
    bool LinuxDirectory::CreateDirectory(const Path& path)
    {
      auto Create = [](const Path& dir)
      {
        return mkdir(
              dir.ToString().c_str(),
              S_IRWXU | S_IRWXG | S_IROTH) == 0;
      };

      Vector<Path> directories = path.GetDirectories();

      for (size_t i = 0; i < directories.size(); ++i)
      {
        const Path& p = directories.at(i);
        const String& ps = p.ToString();

        if (Exists(p) == false)
        {
          if (Create(p) == false)
          {
            return false;
          }
        }
      }

      return Create(path);
    }

    //--------------------------------------------------------------------------
    Vector<Path> LinuxDirectory::EnumerateChildren(const Path& path)
    {
      Vector<Path> result;

      struct dirent* entry;
      DIR* dp;

      Path start_at = path;

      dp = opendir(start_at.ToString().c_str());

      if (dp == nullptr)
      {
        return result;
      }

      entry = readdir(dp);

      do
      {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
          continue;
        }

        result.push_back(start_at / entry->d_name);
      } while ((entry = readdir(dp)) != nullptr);

      closedir(dp);

      return result;
    }
  }
}
