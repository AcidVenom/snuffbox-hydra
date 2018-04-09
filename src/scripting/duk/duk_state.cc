#include "scripting/duk/duk_state.h"

#include <foundation/auxiliary/logger.h>

#include <duktape.h>

#include <cassert>

namespace snuffbox
{
  namespace scripting
  {
    //--------------------------------------------------------------------------
    DukState::DukState() :
      context_(nullptr)
    {
      
    }

    //--------------------------------------------------------------------------
    void DukState::Initialize()
    {
      context_ = duk_create_heap(
        DukAllocator::DukAllocate, 
        DukAllocator::DukReallocate, 
        DukAllocator::DukDeallocate, 
        &allocator_, 
        FatalErrorHandler);
    }

    //--------------------------------------------------------------------------
    bool DukState::CompileFromSource(const char* ctx, const char* src, bool print)
    {
      duk_push_string(context_, src);
      duk_push_string(context_, ctx);

      bool has_error = duk_pcompile(context_, 0) != 0;
      has_error = has_error == true ? true : duk_pcall(context_, 0) != 0;

      if (print == true || has_error == true)
      {
        const char* result = duk_safe_to_string(context_, -1);

        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kScript,
          has_error == true ? 
          foundation::LogSeverity::kError : foundation::LogSeverity::kDebug,
          "({0}): {1}",
          ctx,
          result);

        duk_pop(context_);
      }
      else if (print == false)
      {
        duk_pop(context_);
      }

      return has_error == false;
    }

    //--------------------------------------------------------------------------
    void DukState::Shutdown()
    {
      if (context_ != nullptr)
      {
        duk_destroy_heap(context_);
        context_ = nullptr;
      }
    }

    //--------------------------------------------------------------------------
    void DukState::FatalErrorHandler(void* udata, const char* msg)
    {
      foundation::Logger::LogVerbosity<1>(
        foundation::LogChannel::kScript,
        foundation::LogSeverity::kFatal,
        msg
        );

      assert(false);
    }
  }
}