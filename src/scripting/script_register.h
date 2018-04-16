#pragma once

#ifdef SNUFF_DUKTAPE
#include "scripting/duk/duk_register.h"
namespace snuffbox
{
  namespace scripting
  {
    using ScriptRegister = DukRegister;
  }
}
#endif
