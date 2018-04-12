#include "scripting/duk/duk_definitions.h"
#include "scripting/duk/duk_function.h"
#include "scripting/duk/duk_wrapper.h"

#include "scripting/script_args.h"

#include <duktape.h>

namespace snuffbox
{
  namespace scripting
  {
    //--------------------------------------------------------------------------
    void DukFunction::Bind(
      duk_context* ctx,
      int stack_idx,
      ScriptFunction func,
      const char* name)
    {
      duk_push_c_function(ctx, &DukCall, DUK_VARARGS);
      duk_push_pointer(ctx, func);
      duk_put_prop_string(ctx, -2, DUK_HIDDEN_CALLBACK);
      duk_put_prop_string(ctx, stack_idx - 1, name);
    }

    //--------------------------------------------------------------------------
    int DukFunction::DukCall(duk_hthread* ctx)
    {
      duk_push_current_function(ctx);
      duk_get_prop_string(ctx, -1, DUK_HIDDEN_CALLBACK);

      ScriptFunction func = 
        reinterpret_cast<ScriptFunction>(duk_to_pointer(ctx, -1));
      duk_pop_2(ctx);

      DukWrapper wrapper = DukWrapper(ctx);

      ScriptArgs args;
      wrapper.GetArguments(&args);

      bool res = func(args);

      if (res == false)
      {
        return DUK_RET_TYPE_ERROR;
      }

      ScriptValue* ret = args.return_value();

      if (ret == nullptr)
      {
        return 0;
      }

      wrapper.PushValue<ScriptValue*>(ret);
      return 1;
    }
  }
}