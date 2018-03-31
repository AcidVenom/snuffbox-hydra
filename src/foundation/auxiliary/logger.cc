#include "foundation/auxiliary/logger.h"

#include <chrono>
#include <ctime>

#ifdef _WIN32
#define localtime(x, y)   \
tm x;                     \
localtime_s(&x, &y);
#else
#define localtime(x, y)   \
tm* temp = localtime(&y); \
tm x = *temp;
#endif

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

    //--------------------------------------------------------------------------
    String Logger::GetTimeStamp()
    {
      std::chrono::time_point<std::chrono::system_clock> tp = 
        std::chrono::system_clock::now();

      std::time_t now = std::chrono::system_clock::to_time_t(tp);
      localtime(time, now);

      auto FormatTime = [](int time)
      {
        String formatted = std::to_string(time).c_str();
        formatted = formatted.size() == 1 ? "0" + formatted : formatted;

        return formatted;
      };

      return 
        FormatTime(time.tm_hour) + ":" + 
        FormatTime(time.tm_min) + ":" + 
        FormatTime(time.tm_sec);
    }
  }
}