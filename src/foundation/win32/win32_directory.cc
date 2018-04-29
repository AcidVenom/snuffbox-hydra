#include "foundation/win32/win32_directory.h"
#include "foundation/win32/win32_include.h"

#include <shellapi.h>

#undef CreateDirectory

namespace snuffbox
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    Win32Directory::Win32Directory() :
      is_ok_(false)
    {

    }

    //--------------------------------------------------------------------------
    Win32Directory::Win32Directory(const Path& path) :
      is_ok_(false)
    {
      Open(path);
    }

    //--------------------------------------------------------------------------
    bool Win32Directory::Open(const Path& path)
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
    bool Win32Directory::Exists(const Path& path)
    {
      DWORD attrib = GetFileAttributesA(path.ToString().c_str());

      return 
        (attrib != INVALID_FILE_ATTRIBUTES && 
        ((attrib & FILE_ATTRIBUTE_DIRECTORY) > 0));
    }

    //--------------------------------------------------------------------------
    void Win32Directory::Remove(const Path& path)
    {
      String stringified = path.ToString();

      if (stringified.size() + 1 > MAX_PATH)
      {
        return;
      }

      size_t len = stringified.size();

      char buffer[MAX_PATH];
      memcpy(buffer, stringified.c_str(), len);
      memset(buffer + len, '\0', sizeof(char));
      memset(buffer + len + 1, '\0', sizeof(char));

      SHFILEOPSTRUCTA file_op = 
      {
        NULL,
        FO_DELETE,
        buffer,
        "",
        FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT,
        false,
        0,
        "" 
      };

      SHFileOperationA(&file_op);
    }

    //--------------------------------------------------------------------------
    bool Win32Directory::is_ok() const
    {
      return is_ok_;
    }

    //--------------------------------------------------------------------------
    const Vector<Path>& Win32Directory::children() const
    {
      return children_;
    }

    //--------------------------------------------------------------------------
    bool Win32Directory::CreateDirectory(const Path& path)
    {
      auto Create = [](const Path& dir)
      {
        return CreateDirectoryA(dir.ToString().c_str(), NULL) == TRUE;
      };

      Vector<Path> directories = path.GetDirectories();

      size_t len = directories.size();
      if (len > 0)
      {
        directories.erase(directories.begin() + len - 1);
        --len;
      }

      for (size_t i = 0; i < len; ++i)
      {
        const Path& p = directories.at(i);
        const String& ps = p.ToString();

        if (ps.find(':') != String::npos && ps.size() == 3)
        {
          continue;
        }

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
    Vector<Path> Win32Directory::EnumerateChildren(const Path& path)
    {
      Vector<Path> result;

      WIN32_FIND_DATAA ffd;
      HANDLE current = INVALID_HANDLE_VALUE;

      Path start_at = path.ToString();

      current = FindFirstFileA((start_at + "/*").ToString().c_str(), &ffd);

      if (current == INVALID_HANDLE_VALUE)
      {
        return result;
      }

      do
      {
        if (strcmp(ffd.cFileName, ".") == 0 || strcmp(ffd.cFileName, "..") == 0)
        {
          continue;
        }

        result.push_back(start_at / ffd.cFileName);
      } while (FindNextFileA(current, &ffd) != 0);

      FindClose(current);

      return result;
    }
  }
}