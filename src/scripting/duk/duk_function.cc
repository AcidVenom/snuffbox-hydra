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
    void DukFunction::GetArguments(
      const DukWrapper& wrapper, 
      uint8_t argc, 
      ScriptArgs* args)
    {
      if (args == nullptr)
      {
        return;
      }

      foundation::Vector<ScriptHandle> argv;

      argv.resize(argc);

      for (uint8_t i = 0; i < argc; ++i)
      {
        argv.at(i) = wrapper.GetValueAt(i);
      }

      *args = ScriptArgs(argv, nullptr);
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

      uint8_t argc = static_cast<uint8_t>(duk_get_top(ctx));

      ScriptArgs args;
      GetArguments(wrapper, argc, &args);

      func(args);

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