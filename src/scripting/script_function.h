#pragma once

namespace snuffbox
{
  namespace scripting
  {
    class ScriptArgs;

    /**
    * @brief A type definition for a scripting free-function pointer
    */
    using ScriptFunction = void(*)(ScriptArgs&);
  }
}