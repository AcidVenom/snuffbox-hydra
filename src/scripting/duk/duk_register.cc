#include "scripting/duk/duk_register.h"
#include "scripting/duk/duk_state.h"
#include "scripting/duk/duk_function.h"

#include <foundation/auxiliary/logger.h>

namespace snuffbox
{
  namespace scripting
  {
    //--------------------------------------------------------------------------
    DukRegister::DukRegister(DukState* state) :
      context_(nullptr)
    {
      foundation::Logger::Assert(state != nullptr, 
        "Attempted to create a DukRegister with an invalid state");

      context_ = state->context();

      foundation::Logger::Assert(context_ != nullptr, 
        "Attempted to create a DukRegister with an context");
    }

    //--------------------------------------------------------------------------
    void DukRegister::RegisterFunctions(ScriptFunctionRegister* reg)
    {
      if (reg == nullptr || reg[0].func == nullptr || reg[0].name == nullptr)
      {
        return;
      }

      duk_context* ctx = context_;

      size_t i = 0;

      while (reg[i].func != nullptr && reg[i].name != nullptr)
      {
        const ScriptFunctionRegister& f = reg[i];
        DukFunction::Bind(ctx, -1, f.func, f.name);
        ++i;
      }
    }

    //--------------------------------------------------------------------------
    void DukRegister::RegisterEnums(ScriptEnum* reg)
    {
      duk_context* ctx = context_;

      for (size_t i = 0; i < 0; ++i)
      {

      }
    }
  }
}