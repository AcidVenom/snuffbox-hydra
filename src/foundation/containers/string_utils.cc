#include "foundation/containers/string_utils.h"

namespace snuffbox
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    StringUtils::StringList StringUtils::Split(const String& str, char delim)
    {
      foundation::String current = "";
      foundation::Vector<foundation::String> split;

      char c = '\0';

      for (int i = 0; i < str.size(); ++i)
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
  }
}