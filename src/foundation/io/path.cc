#include "foundation/io/path.h"
#include "foundation/auxiliary/string_utils.h"
#include "foundation/io/directory.h"

#include <cstring>
#include <cstddef>

namespace snuffbox
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    const char* Path::kVirtualPrefix = "snuff:";

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

      if (is_virtual_ == true)
      {
        return;
      }

      is_directory_ = Directory::Exists(*this);

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
    Path Path::operator/(const String& other) const
    {
      if (is_directory_ == false && extension_.size() > 0)
      {
        return *this;
      }

      String p = path_;
      p += p.size() == 0 ? ConvertSlashes(other) : PrependSlash(other);

      return Path(p);
    }

    //--------------------------------------------------------------------------
    Path Path::operator/(const char* other) const
    {
      return *this / String(other);
    }

    //--------------------------------------------------------------------------
    Path Path::operator/(const Path& other) const
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
    Path Path::operator+(const String& other) const
    {
      return Path(path_ + other);
    }

    //--------------------------------------------------------------------------
    Path Path::operator+(const char* other) const
    {
      return *this + String(other);
    }

    //--------------------------------------------------------------------------
    Path Path::operator+(const Path& other) const
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
    bool Path::operator==(const String& other) const
    {
      return path_ == other;
    }

    //--------------------------------------------------------------------------
    bool Path::operator==(const char* other) const
    {
      return *this == String(other);
    }

    //--------------------------------------------------------------------------
    bool Path::operator==(const Path& other) const
    {
      return *this == other.path_;
    }

    //--------------------------------------------------------------------------
    bool Path::operator!=(const String& other) const
    {
      return (*this == other) == false;
    }

    //--------------------------------------------------------------------------
    bool Path::operator!=(const char* other) const
    {
      return (*this == other) == false;
    }

    //--------------------------------------------------------------------------
    bool Path::operator!=(const Path& other) const
    {
      return (*this == other) == false;
    }

    //--------------------------------------------------------------------------
    const String& Path::ToString() const
    {
      return path_;
    }

    //--------------------------------------------------------------------------
    Path Path::StripPath(const Path& path) const
    {
      String stringified = path.ToString();
      stringified = stringified.size() == 0 ? "" : stringified + '/';

      size_t len = stringified.size();

      char c;
      size_t offset = 0;

      for (size_t i = 0; i < path_.size() && i < len; ++i)
      {
        offset = i;
        c = path_.at(i);

        if (c == stringified.at(i))
        {
          if (i == path_.size() - 1 || i == len - 1)
          {
            ++offset;
          }

          continue;
        }

        break;
      }

      if (offset >= path_.size())
      {
        return "";
      }

      return Path(&path_[offset]);
    }

    //--------------------------------------------------------------------------
    Path Path::NoExtension() const
    {
      if (extension_.size() == 0)
      {
        return path_;
      }

      String no_ext = path_;
      size_t i = foundation::StringUtils::FindLastOf(no_ext, '.');

      if (i == String::npos)
      {
        return no_ext;
      }

      no_ext.erase(no_ext.begin() + i, no_ext.end());

      return no_ext;
    }

    //--------------------------------------------------------------------------
    size_t Path::NumDirectories() const
    {
      if (path_.size() == 0)
      {
        return 0;
      }

      size_t len = StringUtils::NumberOf(path_, '/');

      if (path_.at(0) == '/' && len > 0)
      {
        --len;
      }

      return len;
    }

    //--------------------------------------------------------------------------
    Vector<Path> Path::GetDirectories(const Path& strip) const
    {
      Vector<Path> result;
      if (path_.size() == 0)
      {
        return result;
      }

      Path stripped = StripPath(strip);
      String stripped_path = stripped.path_;

      StringUtils::StringList list = StringUtils::Split(stripped_path, '/');

      size_t len = list.size();
      if (stripped_path.at(0) == '/')
      {
        list.erase(list.begin());

        size_t new_len = list.size();
        if (new_len > 0)
        {
          String& first = list.at(0);
          first = '/' + first;
          len = new_len;
        }
      }
      
      if (len < 1)
      {
        return result;
      }

      result.resize(len);

      for (size_t i = 0; i < len; ++i)
      {
        result.at(i) = "";

        for (size_t j = 0; j <= i; ++j)
        {
          result.at(i) /= list.at(j);
        }
      }

      return result;
    }

    //--------------------------------------------------------------------------
    bool Path::is_virtual() const
    {
      return is_virtual_;
    }

    //--------------------------------------------------------------------------
    bool Path::is_directory() const
    {
      return is_directory_;
    }

    //--------------------------------------------------------------------------
    const String& Path::extension() const
    {
      return extension_;
    }

    //--------------------------------------------------------------------------
    String Path::PrependSlash(const String& str)
    {
      if (str.size() == 0 || str.find(':') != String::npos)
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
      size_t semicolon = copy.find(':');
      bool is_drive = false;

      size_t copy_size = copy.size();
      if (semicolon != String::npos && (copy_size == 2 || copy_size == 3))
      {
        if (copy_size == 2 && copy.at(last) == ':')
        {
          copy += '/';
        }

        is_drive = copy.at(copy.size() - 1) == '/';
      }

      if (is_drive == false && IsSlash(copy.at(last)) == true)
      {
        copy.erase(copy.begin() + last);
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
      size_t len = strlen(kVirtualPrefix);

      if (path.size() < len)
      {
        return false;
      }

      for (size_t i = 0; i < len; ++i)
      {
        if (path.at(i) != kVirtualPrefix[i])
        {
          return false;
        }
      }

      return true;
    }

    //--------------------------------------------------------------------------
    bool Path::GetExtension(const String& str, String* ext)
    {
      size_t pos = StringUtils::FindLastOf(str, '.');
      if (pos == String::npos || str.size() < 2)
      {
        return false;
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