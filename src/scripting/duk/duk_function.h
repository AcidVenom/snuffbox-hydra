#pragma once

#include "scripting/script_function.h"

#include <cinttypes>

struct duk_hthread;

namespace snuffbox
{
  namespace scripting
  {
    class DukWrapper;

    /**
    * @brief Used to wrap the duktape free functions, so that ScriptArgs are
    *        created and the actual (member) function is called
    *
    *
    * @author Daniel Konings
    */
    class DukFunction
    {

    public:

      /**
      * @brief Binds a function under a specified string property of an object 
      *        on the duktape stack
      *
      * The function is bound with a pointer field for userdata, this pointer
      * will point to the actual function of the public function to call.
      *
      * @see ScriptFunction
      *
      * @param[in] ctx The duktape context
      * @param[in] stack_idx The stack index the object to bind to resides at
      * @param[in] func The function to bind
      * @param[in] name The name to bind the function under
      *
      * @remarks The function will be wrapped in a C-style, native, duktape
      *          function which will redirect the arguments from the JavaScript
      *          function call
      */
      static void Bind(
        duk_hthread* ctx,
        int stack_idx,
        ScriptFunction func,
        const char* name);

      /**
      * @brief The actual duktape call, which is to be wrapped with
      *        the public function callback
      */
      static int DukCall(duk_hthread* ctx);
    };
  }
}