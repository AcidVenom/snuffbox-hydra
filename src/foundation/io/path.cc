#include "foundation/io/path.h"

#include <cstring>
#include <cstddef>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

namespace snuffbox
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    const char* Path::kVirtualPrefix_ = "snuff:/";

    //--------------------------------------------------------------------------
    Path::Path() :
      path_(""),
      extension_(""),
      is_virtual_(false),
      is_directory_(true)
    {

    }

    //--------------------------------------------------------------------------
    Path::Path(const String& path) :
      path_(ConvertSlashes(path)),
      extension_(""),
      is_virtual_(false),
      is_directory_(true)
    {
      is_virtual_ = IsVirtualPath(path_);
      is_directory_ = IsDirectory(path_);

      if (is_directory_ == false)
      {
        GetExtension(path_, &extension_);
      }
    }

    //--------------------------------------------------------------------------
    Path::Path(const char* path) :
      Path(String(path))
    {
      
    }

    //--------------------------------------------------------------------------
    void Path::operator/=(const String& other)
    {
      *this = *this / other;
    }

    //--------------------------------------------------------------------------
    void Path::operator/=(const char* other)
    {
      *this /= String(other);
    }

    //--------------------------------------------------------------------------
    void Path::operator/=(const Path& other)
    {
      *this /= other.path_;
    }

    //--------------------------------------------------------------------------
    Path Path::operator/(const String& other)
    {
      if (is_directory_ == false)
      {
        return *this;
      }

      String p = path_;
      p += PrependSlash(other);

      return Path(p);
    }

    //--------------------------------------------------------------------------
    Path Path::operator/(const char* other)
    {
      return *this / String(other);
    }

    //--------------------------------------------------------------------------
    Path Path::operator/(const Path& other)
    {
      return *this / other.path_;
    }

    //--------------------------------------------------------------------------
    void Path::operator+=(const String& other)
    {
      *this = *this + other;
    }

    //--------------------------------------------------------------------------
    void Path::operator+=(const char* other)
    {
      *this += String(other);
    }

    //--------------------------------------------------------------------------
    void Path::operator+=(const Path& other)
    {
      *this += other.path_;
    }

    //--------------------------------------------------------------------------
    Path Path::operator+(const String& other)
    {
      return Path(path_ + other);
    }

    //--------------------------------------------------------------------------
    Path Path::operator+(const char* other)
    {
      return *this + String(other);
    }

    //--------------------------------------------------------------------------
    Path Path::operator+(const Path& other)
    {
      return *this + other.path_;
    }

    //--------------------------------------------------------------------------
    Path Path::operator=(const String& other)
    {
      return Path(other);
    }

    //--------------------------------------------------------------------------
    Path Path::operator=(const char* other)
    {
      return Path(other);
    }

    //--------------------------------------------------------------------------
    bool Path::operator==(const String& other)
    {
      return path_ == other;
    }

    //--------------------------------------------------------------------------
    bool Path::operator==(const char* other)
    {
      return *this == String(other);
    }

    //--------------------------------------------------------------------------
    bool Path::operator==(const Path& other)
    {
      return *this == other.path_;
    }

    //--------------------------------------------------------------------------
    bool Path::operator!=(const String& other)
    {
      return (*this == other) == false;
    }

    //--------------------------------------------------------------------------
    bool Path::operator!=(const char* other)
    {
      return (*this == other) == false;
    }

    //--------------------------------------------------------------------------
    bool Path::operator!=(const Path& other)
    {
      return (*this == other) == false;
    }

    //--------------------------------------------------------------------------
    const String& Path::ToString() const
    {
      return path_;
    }

    //--------------------------------------------------------------------------
    Path Path::StripVirtualPrefix() const
    {
      size_t len = strlen(kVirtualPrefix_);

      if (is_virtual_ == false || path_.size() < len)
      {
        return Path(*this);
      }

      char c;
      size_t offset = 0;

      for (size_t i = 0; i < len; ++i)
      {
        c = path_.at(i);

        if (c == kVirtualPrefix_[i])
        {
          continue;
        }

        offset = i;
        break;
      }

      offset = len;

      return Path(&path_[offset]);
    }

    //--------------------------------------------------------------------------
    bool Path::is_virtual() const
    {
      return is_virtual_;
    }

    //--------------------------------------------------------------------------
    String Path::PrependSlash(const String& str)
    {
      if (str.size() == 0)
      {
        return str;
      }

      String converted = ConvertSlashes(str);

      return '/' + converted;
    }

    //--------------------------------------------------------------------------
    String Path::ConvertSlashes(const String& str)
    {
      if (str.size() == 0)
      {
        return str;
      }

      auto IsSlash = [](char c)
      {
        return c == '/' || c == '\\';
      };

      String copy = str;

      size_t last = copy.size() - 1;
      if (IsSlash(copy.at(last)) == true)
      {
        copy.erase(copy.begin() + last);
      }

      if (IsSlash(copy.at(0)) == true)
      {
        copy.erase(copy.begin());
      }

      char c = '\0';
      for (int i = static_cast<int>(copy.size()) - 1; i >= 0; --i)
      {
        c = copy.at(i);

        if (IsSlash(c) == true)
        {
          if (i - 1 >= 0 && IsSlash(copy.at(i - 1)) == true)
          {
            copy.erase(copy.begin() + i);
            continue;
          }

          if (c == '\\')
          {
            copy.at(i) = '/';
          }
        }
      }

      return copy;
    }

    //--------------------------------------------------------------------------
    bool Path::IsVirtualPath(const String& path)
    {
      size_t len = strlen(kVirtualPrefix_);

      if (path.size() < len)
      {
        return false;
      }

      for (size_t i = 0; i < len; ++i)
      {
        if (path.at(i) != kVirtualPrefix_[i])
        {
          return false;
        }
      }

      return true;
    }

    //--------------------------------------------------------------------------
    bool Path::IsDirectory(const String& str)
    {
      struct stat s;
      if(stat(str.c_str(), &s) == 0)
      {
        return (s.st_mode & S_IFDIR) > 0;
      }

      return false;
    }

    //--------------------------------------------------------------------------
    bool Path::GetExtension(const String& str, String* ext)
    {
      size_t pos = 0;
      if ((pos = str.find('.')) == String::npos || str.size() < 2)
      {
        return false;
      }

      size_t new_pos;

      while (true)
      {
        new_pos = str.find('.', pos + 1);

        if (new_pos == String::npos)
        {
          break;
        }

        pos = new_pos;
      }

      if (pos >= str.size() - 1)
      {
        return false;
      }

      *ext = &str[pos + 1];

      return true;
    }
  }
}