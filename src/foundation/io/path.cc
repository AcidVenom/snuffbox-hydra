#include "foundation/io/path.h"

#include <cstring>
#include <cstddef>

namespace snuffbox
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    const char* Path::kVirtualPrefix_ = "snuff:/";

    //--------------------------------------------------------------------------
    Path::Path() :
      path_(""),
      is_virtual_(false)
    {

    }

    //--------------------------------------------------------------------------
    Path::Path(const String& path) :
      path_(ConvertSlashes(path)),
      is_virtual_(false)
    {
      is_virtual_ = IsVirtualPath(path_);
    }

    //--------------------------------------------------------------------------
    Path::Path(const char* path) :
      path_(ConvertSlashes(path)),
      is_virtual_(false)
    {
      is_virtual_ = IsVirtualPath(path_);
    }

    //--------------------------------------------------------------------------
    void Path::operator/=(const String& other)
    {
      path_ += PrependSlash(other);
    }

    //--------------------------------------------------------------------------
    void Path::operator/=(const Path& other)
    {
      path_ += '/' + other.path_;
    }

    //--------------------------------------------------------------------------
    Path Path::operator/(const String& other)
    {
      return path_ + PrependSlash(other);
    }

    //--------------------------------------------------------------------------
    Path Path::operator/(const Path& other)
    {
      return Path(path_ + '/' + other.path_);
    }

    //--------------------------------------------------------------------------
    void Path::operator+=(const String& other)
    {
      path_ += ConvertSlashes(other);
    }

    //--------------------------------------------------------------------------
    void Path::operator+=(const Path& other)
    {
      path_ += other.path_;
    }

    //--------------------------------------------------------------------------
    Path Path::operator+(const String& other)
    {
      return Path(path_ + other);
    }

    //--------------------------------------------------------------------------
    Path Path::operator+(const Path& other)
    {
      return Path(path_ + other.path_);
    }

    //--------------------------------------------------------------------------
    Path Path::operator=(const String& other)
    {
      return Path(other);
    }

    //--------------------------------------------------------------------------
    bool Path::operator==(const String& other)
    {
      return path_ == other;
    }

    //--------------------------------------------------------------------------
    bool Path::operator==(const Path& other)
    {
      return path_ == other.path_;
    }

    //--------------------------------------------------------------------------
    bool Path::operator!=(const String& other)
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
  }
}