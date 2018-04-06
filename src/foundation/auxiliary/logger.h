#pragma once

#include "foundation/definitions/logging.h"

#include "foundation/containers/string.h"
#include "foundation/containers/vector.h"

#include <glm/glm.hpp>

#include <iostream>
#include <sstream>
#include <cinttypes>

namespace snuffbox
{
  namespace foundation
  {
    /**
    * @brief A logging interface to use throughout the application to log 
    *        messages
    *
    * This logger supplies functionality to format a list of arguments C#-style.
    * Aside from that there is support for different channels so log messages
    * can be filtered accordingly.
    *
    * @author Daniel Konings
    */
    class Logger
    {

    public:

      /**
      * @brief An output stream function pointer to redirect the log output with
      */
      using OutputStream = void(*)(void*, LogChannel, LogSeverity, const String&);

      /**
      * @brief Logs a message to a specified logging stream with a 
      *        specified severity
      *
      * The arguments can be denoted with {n}, where n is the argument number
      * passed into the function. "n" starts at 0.
      *
      * @tparam Args... The arguments to format the message with
      *
      * @param[in] channel The channel to log in
      * @param[in] severity The severity to log with
      * @param[in] format The string to format
      * @param[in] args The arguments to be formatted into the string
      */
      template <typename ... Args>
      static void Log(
        LogChannel channel, 
        LogSeverity severity,
        const char* format, 
        Args... args);

      /**
      * @see Logger::Log
      *
      * @brief This log function simply logs to an unspecified channel
      *        with LogSeverity::kDebug severity
      */
      template <typename ... Args>
      static void Log(const char* format, Args... args);

      /**
      * @see Logger::Log
      *
      * @brief Used to log with a specific verbosity
      *
      * The log only gets redirected to the output if the verbosity level
      * is above or equal to the currently set verbosity in Logger::verbosity_
      */
      template <uint32_t V, typename ... Args>
      static void LogVerbosity(
        LogChannel channel,
        LogSeverity severity,
        const char* format,
        Args... args);

      /**
      * @see Logger::LogVerbosity
      * 
      * @brief This log function simply logs to an unspecified channel,
      *        but with a verbosity level, using LogSeverity::kDebug severity
      */
      template <uint32_t V, typename ... Args>
      static void LogVerbosity(const char* format, Args... args);

      /**
      * @brief Sets the verbosity level of the logger
      *
      * Logs will not be redirected if the current verbosity level is lower
      * or equal to the log's verbosity level.
      *
      * @param[in] verbosity The verbosity level to set
      */
      static void SetVerbosity(uint32_t verbosity);

      /**
      * @brief Redirects the output of the logger to a specified function
      *
      * The forwarded message is already formatted using the variadic arguments
      * specified in the log function.
      *
      * @param[in] func The function to redirect the output to
      * @parma[in] ud The user data to send with the logging function
      */
      static void RedirectOutput(OutputStream func, void* ud);

    protected:

      /**
      * @brief Retrieves the string values of each argument in a variadic
      *        argument list
      *
      * This is the first function of the recursive unroll
      *
      * @tparam T The value to format during this function call
      * @tparam Args... The arguments to format the string with
      *
      * @param[in] stringified The list of strings to append the result to
      * @param[in] value The value to format
      * @param[in] args The arguments to format into the string
      *
      * @return The number of formatted arguments
      */
      template <typename T, typename ... Args>
      static uint32_t GetArguments(
        Vector<String>& stringified,
        const T& value, 
        Args... args);

      /**
      * @brief This is the function that is called when there is only 
      *        1 argument left
      *
      * @see Logger::GetArguments<T, Args...>
      */
      template <typename T>
      static uint32_t GetArguments(
        Vector<String>& stringified,
        const T& value);

      /**
      * @brief This is the function that is called when there are 
      *        no arguments left
      *
      * @see Logger::GetArguments<T, Args...>
      */
      static uint32_t GetArguments(Vector<String>& args);

      /**
      * @brief Converts a value to a string
      *
      * @tparam T The value type to convert
      *
      * @param[in] value The value to convert
      *
      * @return The converted value
      */
      template <typename T>
      static String ToString(const T& value);

      /**
      * @brief Formats a string C#-style with given string arguments
      *
      * @param[in] format The format to format the arguments into
      * @param[in] args The list of stringified arguments
      */
      static String FormatString(
        const char* format, 
        const Vector<String>& args);

      /**
      * @brief Converts a severity to a string value
      *
      * @param[in] severity The severity to convert
      *
      * @return The converted value
      */
      static const char* SeverityToString(LogSeverity verbosity);

      /**
      * @return A timestamp string based on the current system time
      */
      static String GetTimeStamp();

    private:

      static OutputStream stream_; //!< The output stream
      static void* stream_ud_; //!< The user data to pass into the output stream

      static const uint32_t kDefaultVerbosity_; //!< The default verbosity
      static uint32_t verbosity_; //!< The currently set verbosity
    };

    //--------------------------------------------------------------------------
    template <typename ... Args>
    inline void Logger::Log(
      LogChannel channel,
      LogSeverity severity,
      const char* format,
      Args... args)
    {
      Vector<String> args;
      GetArguments(args, eastl::forward<Args>(args)...);

      String message = 
        "[" +
        GetTimeStamp() + "|" +
        String(SeverityToString(severity)) +
        "] " +
        FormatString(format, args);
      
      std::cout << message.c_str() << std::endl;

      if (stream_ != nullptr)
      {
        stream_(stream_ud_, channel, severity, message);
      }
    }

    //--------------------------------------------------------------------------
    template <typename ... Args>
    inline void Logger::Log(const char* format, Args... args)
    {
      Log(
        LogChannel::kUnspecified,
        LogSeverity::kDebug,
        format,
        eastl::forward<Args>(args)...);
    }

    //--------------------------------------------------------------------------
    template <uint32_t V, typename ... Args>
    inline void Logger::LogVerbosity(
      LogChannel channel,
      LogSeverity severity,
      const char* format,
      Args... args)
    {
      static_assert(V != 0, "Logging with a verbosity of 0 is redundant");

      if (V <= verbosity_)
      {
        Log(channel, severity, format, eastl::forward<Args>(args)...);
      }
    }

    //--------------------------------------------------------------------------
    template <uint32_t V, typename ... Args>
    inline void Logger::LogVerbosity(const char* format, Args... args)
    {
      LogVerbosity<V, Args...>(
        LogChannel::kUnspecified,
        LogSeverity::kDebug,
        format,
        eastl::forward<Args>(args)...);
    }

    //--------------------------------------------------------------------------
    template <typename T, typename ... Args>
    inline uint32_t Logger::GetArguments(
      Vector<String>& stringified,
      const T& value, 
      Args... args)
    {
      stringified.push_back(ToString<T>(value));
      return GetArguments(stringified, eastl::forward<Args>(args)...) + 1;
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline uint32_t Logger::GetArguments(
      Vector<String>& stringified,
      const T& value)
    {
      stringified.push_back(ToString<T>(value));
      return 1;
    }

    //--------------------------------------------------------------------------
    inline uint32_t Logger::GetArguments(Vector<String>& formatted)
    {
      return 0;
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline String Logger::ToString(const T& value)
    {
      std::ostringstream stream;
      stream << value;

      return stream.str().c_str();
    }

    //--------------------------------------------------------------------------
    template <>
    inline String Logger::ToString(const bool& value)
    {
      return value == true ? "true" : "false";
    }

    //--------------------------------------------------------------------------
    template <>
    inline String Logger::ToString(const String& value)
    {
      return value;
    }

    //--------------------------------------------------------------------------
    template <>
    inline String Logger::ToString(const glm::vec2& value)
    {
      return 
        "[" + 
        ToString<float>(value.x) + ", " + 
        ToString<float>(value.y) + 
        "]";
    }

    //--------------------------------------------------------------------------
    template <>
    inline String Logger::ToString(const glm::vec3& value)
    {
      return
        "[" +
        ToString<float>(value.x) + ", " +
        ToString<float>(value.y) + ", " +
        ToString<float>(value.z) +
        "]";
    }

    //--------------------------------------------------------------------------
    template <>
    inline String Logger::ToString(const glm::vec4& value)
    {
      return
        "[" +
        ToString<float>(value.x) + ", " +
        ToString<float>(value.y) + ", " +
        ToString<float>(value.z) + ", " +
        ToString<float>(value.w) +
        "]";
    }
  }
}