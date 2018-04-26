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
      SHFILEOPSTRUCTA file_op = 
      {
        NULL,
        FO_DELETE,
        path.ToString().c_str(),
        "",
        FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT,
        false,
        0,
        "" 
      };

      SHFileOperationA(&file_op);
    }

    //--------------------------------------------------------------------------
    bool Win32Directory::CreateDirectory(const Path& path)
    {
      auto Create = [](const Path& dir)
      {
        return CreateDirectoryA(dir.ToString().c_str(), NULL) == TRUE;
      };

      Vector<Path> directories = path.GetDirectories();

      for (size_t i = 0; i < directories.size(); ++i)
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
      return Vector<Path>();
    }
  }
}