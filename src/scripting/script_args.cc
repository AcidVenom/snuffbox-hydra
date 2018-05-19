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
      return_value_(nullptr)
    {

    }

    //--------------------------------------------------------------------------
    ScriptArgs::ScriptArgs(
      const foundation::Vector<ScriptHandle>& args,
      void* callee)
      :
      arguments_(args),
      callee_(callee),
      return_value_(nullptr)
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
    template <>
    bool ScriptArgs::Get(
      uint8_t idx, 
      bool def, 
      if_n_number_and_enum<bool>*) const
    {
      return 
        GetImpl<bool, ScriptBoolean, ScriptValueTypes::kBoolean>(idx, def);
    }

    //--------------------------------------------------------------------------
    template <>
    foundation::String ScriptArgs::Get(
      uint8_t idx, 
      foundation::String def, 
      if_n_number_and_enum<foundation::String>*) const
    {
      return 
        GetImpl<
        foundation::String, 
        ScriptString, 
        ScriptValueTypes::kString>(idx, def);
    }

    //--------------------------------------------------------------------------
    template <>
    inline glm::vec2 ScriptArgs::Get(
      uint8_t idx, 
      glm::vec2 def, 
      if_n_number_and_enum<glm::vec2>*) const
    {
      return GetVectorValue(idx, def);
    }

    //--------------------------------------------------------------------------
    template <>
    inline glm::vec3 ScriptArgs::Get(
      uint8_t idx, 
      glm::vec3 def, 
      if_n_number_and_enum<glm::vec3>*) const
    {
      return GetVectorValue(idx, def);
    }

    //--------------------------------------------------------------------------
    template <>
    inline glm::vec4 ScriptArgs::Get(
      uint8_t idx, 
      glm::vec4 def, 
      if_n_number_and_enum<glm::vec4>*) const
    {
      return GetVectorValue(idx, def);
    }

    //--------------------------------------------------------------------------
    void ScriptArgs::AddReturnPointer(
      void* ptr,
      const foundation::String& type_name)
    {
      ScriptObjectHandle h = ScriptValue::CreateObject();
      h->SetPointer(ptr, type_name.c_str());

      AddReturnValue<ScriptObjectHandle>(h);
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