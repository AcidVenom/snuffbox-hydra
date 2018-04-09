#include "scripting/duk/duk_wrapper.h"

#include <foundation/auxiliary/logger.h>

namespace snuffbox
{
  namespace scripting
  {
    //--------------------------------------------------------------------------
    DukWrapper::DukWrapper(duk_context* ctx) :
      context_(ctx)
    {
      foundation::Logger::Assert(
        context_ != nullptr,
        "DukWrapper context_ cannnot be nullptr");
    }
  }
}