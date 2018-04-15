#include "engine/auxiliary/debug.h"

#ifndef SNUFF_NSCRIPTING
#include <sparsed/debug.gen.cc>
#endif

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    void Debug::Log(const foundation::String& str)
    {
      foundation::Logger::LogVerbosity<1>(
        foundation::LogChannel::kScript,
        foundation::LogSeverity::kDebug, str.c_str());
    }

    //--------------------------------------------------------------------------
    void Debug::LogInfo(const foundation::String& str)
    {
      foundation::Logger::LogVerbosity<1>(
        foundation::LogChannel::kScript,
        foundation::LogSeverity::kInfo, str.c_str());
    }

    //--------------------------------------------------------------------------
    void Debug::LogWarning(const foundation::String& str)
    {
      foundation::Logger::LogVerbosity<1>(
        foundation::LogChannel::kScript,
        foundation::LogSeverity::kWarning, str.c_str());
    }

    //--------------------------------------------------------------------------
    void Debug::LogSuccess(const foundation::String& str)
    {
      foundation::Logger::LogVerbosity<1>(
        foundation::LogChannel::kScript,
        foundation::LogSeverity::kSuccess, str.c_str());
    }

    //--------------------------------------------------------------------------
    void Debug::LogError(const foundation::String& str)
    {
      foundation::Logger::LogVerbosity<1>(
        foundation::LogChannel::kScript,
        foundation::LogSeverity::kError, str.c_str());
    }

    //--------------------------------------------------------------------------
    void Debug::LogFatal(const foundation::String& str)
    {
      foundation::Logger::LogVerbosity<1>(
        foundation::LogChannel::kScript,
        foundation::LogSeverity::kFatal, str.c_str());
    }

    //--------------------------------------------------------------------------
    void Debug::Assert(bool exp, const foundation::String& str)
    {
      foundation::Logger::Assert(exp, str.c_str());
    }
  }
}