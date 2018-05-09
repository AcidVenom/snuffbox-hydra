#include "foundation/auxiliary/logger.h"

#include <chrono>
#include <ctime>
#include <cassert>

#ifdef SNUFF_WIN32
#define localtime(x, y)   \
tm x;                     \
localtime_s(&x, &y);
#elif defined (SNUFF_LINUX)
#define localtime(x, y)   \
tm* temp = localtime(&y); \
tm x = *temp;
#else
#error "Unknown compiler platform"
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
    void Logger::Assert(bool expr, const char* msg)
    {
      if (expr == false)
      {
        Log(LogChannel::kUnspecified, LogSeverity::kFatal, 
          "\nAssertion failure\n================================\n\n{0}", msg);
      }

      assert(expr && "Assertion failure");
    }

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
        return "?";

      case LogSeverity::kWarning:
        return "!";

      case LogSeverity::kSuccess:
        return "$";

      case LogSeverity::kError:
        return "#";

      case LogSeverity::kFatal:
        return "###";
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