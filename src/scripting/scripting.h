#pragma once

#include "scripting/script_callback.h"

#ifdef SNUFF_DUKTAPE
namespace snuffbox
{
  namespace scripting
  {
#include "scripting/duk/duk_state.h"
    using ScriptState = DukState;
  }
}
#elif !defined SNUFF_NSCRIPTING
#error "Unsupported scripting implementation"
#endif