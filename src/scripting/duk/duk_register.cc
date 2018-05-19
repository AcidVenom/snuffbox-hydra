#include "scripting/duk/duk_register.h"
#include "scripting/duk/duk_state.h"

#include <foundation/auxiliary/logger.h>

namespace snuffbox
{
  namespace scripting
  {
    //--------------------------------------------------------------------------
    DukRegister::DukRegister(DukState* state) :
      state_(state),
      context_(nullptr)
    {
      foundation::Logger::Assert(state != nullptr, 
        "Attempted to create a DukRegister with an invalid state");

      context_ = state->context();

      foundation::Logger::Assert(context_ != nullptr, 
        "Attempted to create a DukRegister with an context");
    }

    //--------------------------------------------------------------------------
    void DukRegister::RegisterEnum(const ScriptEnum& reg)
    {
      duk_context* ctx = context_;

      size_t i = 0;

      duk_push_global_object(ctx);
      duk_push_object(ctx);

      ScriptEnumRegister& e = reg.keys[i];

      while(e.key != nullptr)
      {
        duk_push_number(ctx, static_cast<double>(e.value));
        duk_put_prop_string(ctx, -2, e.key);

        e = reg.keys[++i];
      }

      duk_put_prop_string(ctx, -2, reg.name);
      duk_pop(ctx);
    }
  }
}