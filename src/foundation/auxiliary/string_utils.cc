#include "foundation/auxiliary/string_utils.h"

namespace snuffbox
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    StringUtils::StringList StringUtils::Split(const String& str, char delim)
    {
      String current = "";
      Vector<String> split;

      char c = '\0';

      size_t size = str.size();
      for (int i = 0; i < size; ++i)
      {
        c = str.at(i);

        if (c == delim)
        {
          split.push_back(current);
          current = "";
          continue;
        }

        current += c;
      }

      if (current.size() != 0)
      {
        split.push_back(current);
      }

      if (split.size() == 0)
      {
        split.push_back(str);
      }

      return split;
    }

    //--------------------------------------------------------------------------
    int StringUtils::Contains(const String& str, const char* token)
    {
      size_t tl = strlen(token);
      size_t counter = 0;

      for (size_t i = 0; i < str.size(); ++i)
      {
        while (
          counter < tl &&
          i < str.size() &&
          str.at(i) == token[counter])
        {
          ++i;
          ++counter;
        }

        if (counter == tl)
        {
          return static_cast<int>(i - tl);
        }

        counter = 0;
      }

      return -1;
    }

    //--------------------------------------------------------------------------
    size_t StringUtils::FindLastOf(const String& str, char token)
    {
      size_t pos = 0;
      if ((pos = str.find(token)) == String::npos)
      {
        return String::npos;
      }

      size_t new_pos;

      while (true)
      {
        new_pos = str.find(token, pos + 1);

        if (new_pos == String::npos)
        {
          break;
        }

        pos = new_pos;
      }

      return pos;
    }

    //--------------------------------------------------------------------------
    size_t StringUtils::NumberOf(const String& str, char token)
    {
      size_t count = 0;
      for (size_t i = 0; i < str.size(); ++i)
      {
        if (str.at(i) == token)
        {
          ++count;
        }
      }

      return count;
    }
  }
}
