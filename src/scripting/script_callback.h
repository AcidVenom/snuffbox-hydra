#pragma once

#ifdef SNUFF_DUKTAPE
#include "scripting/duk/duk_callback.h"
namespace snuffbox
{
  namespace scripting
  {
    template <typename ... Args>
    using ScriptCallback = DukCallback<Args...>;
  }
}
#elif !defined SNUFF_NSCRIPTING 
#error "Unsupported scripting implementation"
#endif