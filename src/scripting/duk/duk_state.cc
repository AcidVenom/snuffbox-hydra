#include "scripting/duk/duk_state.h"

#include <foundation/auxiliary/logger.h>
#include <foundation/containers/string_utils.h>

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
    void DukState::FatalErrorHandler(void* udata, const char* msg)
    {
      foundation::Logger::Assert(false, msg);
    }

    //--------------------------------------------------------------------------
    duk_ret_t print(duk_context* ctx)
    {
      duk_int_t argc = duk_get_top(ctx);
      const char* value = duk_safe_to_string(ctx, -1);
      printf("%s\n", value);

      return 0;
    }

    //--------------------------------------------------------------------------
    bool DukState::Initialize()
    {
      context_ = duk_create_heap(
        DukAllocator::DukAllocate, 
        DukAllocator::DukReallocate, 
        DukAllocator::DukDeallocate, 
        &allocator_, 
        FatalErrorHandler);

      if (context_ == nullptr)
      {
        return false;
      }

      duk_push_c_function(context_, print, 1);
      duk_put_global_string(context_, "print");

      return true;
    }

    //--------------------------------------------------------------------------
    bool DukState::CompileFromSource(
      const char* ctx, 
      const char* src, 
      bool print)
    {
      duk_push_string(context_, src);
      duk_push_string(context_, ctx);

      bool has_error = false;
      bool compiled = true;

      if (duk_pcompile(context_, 0) != 0)
      {
        LogLastError("Compilation error at {0}:{1}\n\n{2}");
        has_error = true;
        compiled = false;
      }

      has_error = has_error == true ? true : duk_pcall(context_, 0) != 0;

      if (print == true && has_error == false)
      {
        const char* result = duk_safe_to_string(context_, -1);

        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kScript,
          foundation::LogSeverity::kDebug,
          "{0}",
          result);
      }
      else if (has_error == true && compiled == true)
      {
        LogLastError("Runtime error at {0}:{1}\n\n{2}");
      }

      duk_pop(context_);

      return has_error == false;
    }

    //--------------------------------------------------------------------------
    void DukState::LogLastError(const char* format)
    {
      duk_get_prop_string(context_, -1, "fileName");
      duk_get_prop_string(context_, -2, "lineNumber");
      duk_get_prop_string(context_, -3, "stack");

      const char* name = duk_safe_to_string(context_, -3);
      const char* line = duk_safe_to_string(context_, -2);
      const char* message = duk_safe_to_string(context_, -1);

      foundation::StringUtils::StringList split = 
        foundation::StringUtils::Split(message, '\n');

      foundation::String stack = "";

      for (size_t i = 0; i < split.size(); ++i)
      {
        if (
          foundation::StringUtils::Contains(split.at(i), "duk_") >= 0 ||
          foundation::StringUtils::Contains(split.at(i), "anon") >= 0)
        {
          continue;
        }

        stack += split.at(i) + '\n';
      }

      foundation::Logger::LogVerbosity<1>(
        foundation::LogChannel::kScript,
        foundation::LogSeverity::kError,
        format,
        name,
        line,
        stack);

      duk_pop_3(context_);
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
    duk_hthread* DukState::context() const
    {
      return context_;
    }
  }
}