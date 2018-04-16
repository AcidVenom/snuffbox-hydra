#pragma once

#include "scripting/script_function.h"

#include "scripting/duk/duk_definitions.h"
#include "scripting/duk/duk_function.h"
#include "scripting/duk/duk_wrapper.h"

#include "scripting/script_args.h"

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
      * @tparam T The callee's type
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
      template <typename T>
      static void Bind(
        duk_context* ctx,
        int stack_idx,
        ScriptFunction func,
        const char* name);

      /**
      * @brief The actual duktape call, which is to be wrapped with
      *        the public function callback
      *
      * @tparam T The callee's type
      */
      template <typename T>
      static int DukCall(duk_context* ctx);
    };

    //--------------------------------------------------------------------------
    template <typename T>
    inline void DukFunction::Bind(
      duk_context* ctx,
      int stack_idx,
      ScriptFunction func,
      const char* name)
    {
      duk_push_c_function(ctx, &DukCall<T>, DUK_VARARGS);
      duk_push_pointer(ctx, func);
      duk_put_prop_string(ctx, -2, DUK_HIDDEN_CALLBACK);
      duk_put_prop_string(ctx, stack_idx - 1, name);
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline int DukFunction::DukCall(duk_hthread* ctx)
    {
      duk_push_current_function(ctx);
      duk_get_prop_string(ctx, -1, DUK_HIDDEN_CALLBACK);

      ScriptFunction func = 
        reinterpret_cast<ScriptFunction>(duk_to_pointer(ctx, -1));
      duk_pop_2(ctx);

      DukWrapper wrapper = DukWrapper(ctx);

      ScriptArgs args;
      wrapper.GetArguments<T>(&args);

      bool res = func(args);

      if (res == false)
      {
        return DUK_RET_TYPE_ERROR;
      }

      ScriptValue* ret = args.return_value();
      ScriptArgs::ReturnPtr ptr = args.return_ptr();

      if (ret == nullptr && ptr.ptr == nullptr)
      {
        return 0;
      }
      else if (ret != nullptr)
      {
        wrapper.PushValue<ScriptValue*>(ret);
      }
      else
      {
        wrapper.PushPointer(args.callee(), ptr.ptr, ptr.type.c_str());
      }

      return 1;
    }
  }
}