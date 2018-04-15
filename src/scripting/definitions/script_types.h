#pragma once

namespace snuffbox
{
  namespace scripting
  {
    /**
    * @brief The different type of script values that exist
    */
    enum class ScriptValueTypes
    {
      kNull, //!< Null/undefined value
      kBoolean, //!< Boolean values
      kNumber, //!< Numerical values, they are always double
      kString, //!< String values stored as an EASTL string
      kObject, //!< Object values by key/value pairs
      kArray, //!< Array values by indices
    };
  }
}