#pragma once

namespace snuffbox
{
  namespace scripting
  {
    class ScriptArgs;

    /**
    * @brief A type definition for a scripting free-function pointer
    *
    * @param[in] args The arguments that were parsed from the script state
    *
    * @return Was the call a success?
    */
    using ScriptFunction = bool(*)(ScriptArgs& args);
  }
}