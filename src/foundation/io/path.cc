#include "foundation/io/path.h"

namespace snuffbox
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    Path::Path() :
      path_("")
    {

    }

    //--------------------------------------------------------------------------
    Path::Path(const String& path) :
      path_(ConvertSlashes(path))
    {

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
  }
}