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
    Logger::OutputStream Logger::stream_ = nullptr;
    void* Logger::stream_ud_ = nullptr;

    const uint32_t Logger::kDefaultVerbosity_ = 1;
    uint32_t Logger::verbosity_ = Logger::kDefaultVerbosity_;

    //--------------------------------------------------------------------------
    void Logger::SetVerbosity(uint32_t verbosity)
    {
      verbosity_ = verbosity;
    }

    //--------------------------------------------------------------------------
    void Logger::RedirectOutput(OutputStream func, void* ud)
    {
      stream_ = func;
      stream_ud_ = ud;
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
    const char* Logger::SeverityToString(LogSeverity verbosity)
    {
      switch (verbosity)
      {
      case LogSeverity::kDebug:
        return "-";

      case LogSeverity::kInfo:
        return "Info";

      case LogSeverity::kWarning:
        return "Warning";

      case LogSeverity::kSuccess:
        return "Success";

      case LogSeverity::kError:
        return "Error";

      case LogSeverity::kFatal:
        return "FATAL";
      }

      return "<unknown>";
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