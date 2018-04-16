#include "scripting/script_args.h"

#include <foundation/auxiliary/logger.h>

#include <cstring>
#include <ctype.h>

namespace snuffbox
{
  namespace scripting
  {
    //--------------------------------------------------------------------------
    ScriptArgs::ScriptArgs() :
      callee_(nullptr),
      return_value_(nullptr),
      return_ptr_({nullptr, ""})
    {

    }

    //--------------------------------------------------------------------------
    ScriptArgs::ScriptArgs(
      const foundation::Vector<ScriptHandle>& args,
      void* callee)
      :
      arguments_(args),
      callee_(callee),
      return_value_(nullptr),
      return_ptr_({nullptr, ""})
    {

    }

    //--------------------------------------------------------------------------
    bool ScriptArgs::Check(const char* format) const
    {
      size_t len = strlen(format);

      auto GetArgumentType = [=](size_t idx)
      {
        if (idx >= arguments_.size())
        {
          return ScriptValueTypes::kNull;
        }

        return arguments_.at(idx)->type();
      };

      char c;
      ScriptValueTypes expected, type;

      for (size_t i = 0; i < len; ++i)
      {
        c = static_cast<char>(toupper(format[i]));
        type = GetArgumentType(i);

        switch (c)
        {
        case 'N':
          expected = ScriptValueTypes::kNumber;
          break;

        case 'B':
          expected = ScriptValueTypes::kBoolean;
          break;

        case 'S':
          expected = ScriptValueTypes::kString;
          break;

        case 'O':
          expected = ScriptValueTypes::kObject;
          break;

        case 'A':
          expected = ScriptValueTypes::kArray;
          break;

        default:
          foundation::Logger::LogVerbosity<2>(
            foundation::LogChannel::kScript,
            foundation::LogSeverity::kWarning,
            "Unexpected format '{0}' while checking arguments",
            c);

          return false;
        }

        if (CheckArg(expected, type, static_cast<uint8_t>(i)) == false)
        {
          return false;
        }
      }

      return true;
    }

    //--------------------------------------------------------------------------
    uint8_t ScriptArgs::Count() const
    {
      return static_cast<uint8_t>(arguments_.size());
    }

    //--------------------------------------------------------------------------
    void* ScriptArgs::callee() const
    {
      return callee_;
    }

    //--------------------------------------------------------------------------
    ScriptValue* ScriptArgs::return_value() const
    {
      if (return_value_ == nullptr)
      {
        return nullptr;
      }

      return return_value_.get();
    }

    //--------------------------------------------------------------------------
    ScriptArgs::ReturnPtr ScriptArgs::return_ptr() const
    {
      return return_ptr_;
    }

    //--------------------------------------------------------------------------
    bool ScriptArgs::HasArgument(uint8_t idx) const
    {
      return idx < static_cast<uint8_t>(arguments_.size());
    }

    //--------------------------------------------------------------------------
    ScriptValue* ScriptArgs::GetArgument(uint8_t idx) const
    {
      if (HasArgument(idx) == false)
      {
        return nullptr;
      }

      return arguments_.at(idx).get();
    }

    //--------------------------------------------------------------------------
    bool ScriptArgs::CheckArg(
      ScriptValueTypes expected, 
      ScriptValueTypes type,
      uint8_t idx)
    {
      if (expected != type)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kScript,
          foundation::LogSeverity::kError,
          "Expected argument of type '{0}', but got '{1}', for argument {2}",
          ScriptValue::TypeToString(expected),
          ScriptValue::TypeToString(type),
          static_cast<uint32_t>(idx)
          );
        return false;
      }

      return true;
    }
  }
}