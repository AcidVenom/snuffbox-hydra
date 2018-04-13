#pragma once

namespace snuffbox
{
  namespace foundation
  {
    /**
    * @brief The different logging severity levels that exist
    */
    enum class LogSeverity
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
    enum class LogChannel
    {
      kUnspecified,
      kEngine,
      kEditor,
      kPlayer,
      kScript,
      kBuilder,
      kNumChannels
    };
  }
}