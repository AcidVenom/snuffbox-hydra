#pragma once

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
      * @brief The different verbosity levels that exist
      */
      enum struct Verbosity
      {
        kDebug,
        kInfo,
        kWarning,
        kSuccess,
        kError,
        kFatal
      };

      /**
      * @brief The different logging channels that exist
      */
      enum struct Channel
      {
        kUnspecified,
        kEngine,
        kEditor,
        kPlayer,
        kScript,
        kBuilder,
        kNumChannels
      };

      /**
      * @brief An output stream function pointer to redirect the log output with
      */
      using OutputStream = void(*)(void*, Channel, Verbosity, const String&);

      /**
      * @brief Logs a message to a specified logging stream with a 
      *        specified verbosity
      *
      * The arguments can be denoted with {n}, where n is the argument number
      * passed into the function. "n" starts at 0.
      *
      * @tparam Args... The arguments to format the message with
      *
      * @param[in] channel The channel to log in
      * @param[in] verbosity The verbosity to log with
      * @param[in] format The string to format
      * @param[in] args The arguments to be formatted into the string
      */
      template <typename ... Args>
      static void Log(
        Channel channel, 
        Verbosity verbosity, 
        const char* format, 
        Args... args);

      /**
      * @see Logger::Log
      *
      * @brief This log function simply logs to an unspecified channel
      *        with Logger::Verbosity::kDebug verbosity
      */
      template <typename ... Args>
      static void Log(const char* format, Args... args);

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
      * @brief Formats a string C#-style with given string arguments
      *
      * @param[in] format The format to format the arguments into
      * @param[in] args The list of stringified arguments
      */
      static String FormatString(
        const char* format, 
        const Vector<String>& args);

      /**
      * @brief Converts a verbosity to a string value
      *
      * @param[in] verbosity The verbosity to convert
      *
      * @return The converted value
      */
      static const char* VerbosityToString(Verbosity verbosity);

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
      * @return A timestamp string based on the current system time
      */
      static String GetTimeStamp();

    private:

      static OutputStream stream_; //!< The output stream
      static void* stream_ud_; //!< The user data to pass into the output stream
    };

    //--------------------------------------------------------------------------
    template <typename ... Args>
    inline void Logger::Log(
      Channel channel,
      Verbosity verbosity,
      const char* format,
      Args... args)
    {
      Vector<String> args;
      GetArguments(args, eastl::forward<Args>(args)...);

      String message = 
        "[" +
        GetTimeStamp() + "|" +
        String(VerbosityToString(verbosity)) +
        "] " +
        FormatString(format, args);
      
      std::cout << message.c_str() << std::endl;

      if (stream_ != nullptr)
      {
        stream_(stream_ud_, channel, verbosity, message);
      }
    }

    //--------------------------------------------------------------------------
    template <typename ... Args>
    inline void Logger::Log(const char* format, Args... args)
    {
      Log(
        Channel::kUnspecified,
        Verbosity::kDebug,
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