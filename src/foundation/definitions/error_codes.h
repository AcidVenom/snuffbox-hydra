#pragma once

namespace snuffbox
{
  namespace foundation
  {
    /**
    * @brief The different error codes that exist
    */
    enum class ErrorCodes
    {
      kSuccess = 0,
      kGLFWInitializationFailed,
      kCreateWindowError,
      kScriptInitializationFailed,
      kNoRendererCreated,
      kRendererInitializationFailed,
      kBuilderInitializationFailed,
      kUnknown
    };
  }
}