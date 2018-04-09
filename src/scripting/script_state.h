#pragma once

#include <foundation/containers/string.h>

namespace snuffbox
{
  namespace scripting
  {
    /**
    * @brief The interface for every native scripting API to use
    *
    * Currently only JavaScript is supported through duktape,
    * however the system should be designed so that more scripting
    * languages could be added (e.g. Mono, LuaJIT, etc.)
    *
    * @author Daniel Konings
    */
    class IScriptState
    {

    public:

      /**
      * @brief Initializes the native scripting API
      *
      * @return Was the initialization a success?
      */
      virtual bool Initialize() = 0;

      /**
      * @brief Compiles a piece of scripting code from a string value
      *
      * Whether the context is used is completely scripting API dependent,
      * but I know for a fact libraries like V8 and LuaJIT use this context
      * for error logging.
      *
      * @param[in] ctx The context this script was excuted in (e.g. a filename)
      * @param[in] src The code to execute
      * @param[in] print Should the result be printed to the console?
      *
      * @return Was the execution succesful?
      */
      virtual bool CompileFromSource(const char* ctx, const char* src, bool print) = 0;

      /**
      * @brief Shuts down the native scripting API after usage
      */
      virtual void Shutdown() = 0;
    };
  }
}