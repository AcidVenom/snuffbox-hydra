#include "foundation/auxiliary/logger.h"

namespace snuffbox
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    const char* Logger::VerbosityToString(Verbosity verbosity)
    {
      switch (verbosity)
      {
      case Verbosity::kDebug:
        return "-";

      case Verbosity::kInfo:
        return "?";

      case Verbosity::kWarning:
        return "~";

      case Verbosity::kSuccess:
        return "$";

      case Verbosity::kError:
        return "!";

      case Verbosity::kFatal:
        return "FATAL";
      }

      return "<unknown>";
    }

    //--------------------------------------------------------------------------
    String Logger::FormatString(const char* format, const Vector<String>& args)
    {
      size_t len = strlen(format);

      String formatted = "";
      String arg = "";

      int argc;

      for (size_t i = 0; i < len; ++i)
      {
        if (format[i] == '{')
        {
          ++i;
          arg = "";

          while (format[i] != '}' && i < len)
          {
            arg += format[i];
            ++i;
          }

          argc = std::atoi(arg.c_str());
          
          if (argc < args.size())
          {
            formatted += args.at(argc);
          }
          else
          {
            formatted += "<out of bounds>";
          }

          continue;
        }

        formatted += format[i];
      }

      return formatted;
    }
  }
}