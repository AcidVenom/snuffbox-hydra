#pragma once

#include <foundation/auxiliary/logger.h>

#include <scripting/script_class.h>

namespace snuffbox
{
  namespace engine
  {
    /**
    * @brief Provides debug functionalities throughout the engine project
    *
    * @author Daniel Konings
    */
    SCRIPT_CLASS() class Debug : public scripting::ScriptClass
    {

    public:

      SCRIPT_NAME(Debug);

      /**
      * @see Logger::Log
      *
      * @brief Short hand for logging to the engine channel with a severity
      *
      * @remarks This uses a log verbosity of 1
      */
      template <typename ... Args>
      static void Log(
        foundation::LogSeverity severity,
        const char* format,
        Args... args);

      /**
      * @see Debug::Log
      *
      * @brief Short hand for logging to the engine channel with a severity
      *        and verbosity
      */
      template <uint32_t V, typename ... Args>
      static void LogVerbosity(
        foundation::LogSeverity severity,
        const char* format,
        Args... args);

      SCRIPT_FUNC() static void Log(const foundation::String& str);
      SCRIPT_FUNC() static void LogInfo(const foundation::String& str);
      SCRIPT_FUNC() static void LogWarning(const foundation::String& str);
      SCRIPT_FUNC() static void LogSuccess(const foundation::String& str);
      SCRIPT_FUNC() static void LogError(const foundation::String& str);
      SCRIPT_FUNC() static void LogFatal(const foundation::String& str);
      SCRIPT_FUNC() static void Assert(bool exp, const foundation::String& str);
    };

    //--------------------------------------------------------------------------
    template <typename ... Args>
    inline void Debug::Log(
      foundation::LogSeverity severity,
      const char* format,
      Args... args)
    {
      LogVerbosity<1>(severity, format, eastl::forward<Args>(args)...);
    }

    //--------------------------------------------------------------------------
    template <uint32_t V, typename ... Args>
    inline void Debug::LogVerbosity(
      foundation::LogSeverity severity,
      const char* format,
      Args... args)
    {
      foundation::Logger::LogVerbosity<V>(
        foundation::LogChannel::kEngine,
        severity,
        format,
        eastl::forward<Args>(args)...);
    }
  }
}