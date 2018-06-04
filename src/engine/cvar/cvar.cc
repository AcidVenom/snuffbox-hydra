#include "engine/cvar/cvar.h"
#include "engine/auxiliary/debug.h"

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    CVarValue::Range::Range() :
      min(0.0),
      max(0.0),
      has_min(false),
      has_max(false)
    {

    }

    //--------------------------------------------------------------------------
    CVarValue::CVarValue(const char* name, const char* description) :
      name_(name),
      description_(description)
    {

    }

    //--------------------------------------------------------------------------
    void CVarValue::Set(const char* value)
    {
      if (ParseFrom(value) == false)
      {
        Debug::LogVerbosity<1>(
          foundation::LogSeverity::kError,
          "Invalid value passed into CVar with name '{0}', usage:\n\t{1}",
          name_, 
          Usage());
      }
    }

    //--------------------------------------------------------------------------
    const foundation::String& CVarValue::name() const
    {
      return name_;
    }

    //--------------------------------------------------------------------------
    const foundation::String& CVarValue::description() const
    {
      return description_;
    }

    //--------------------------------------------------------------------------
    template <>
    bool CVar<bool>::ParseFrom(const char* value)
    {
      size_t len = strlen(value);

      if (len == 0 || len > 5)
      {
        return false;
      }

      char buffer[6];

      for (size_t i = 0; i < len; ++i)
      {
        buffer[i] = static_cast<char>(tolower(value[i]));
      }

      buffer[len] = '\0';

      if (strcmp(buffer, "true") == 0)
      {
        value_ = true;
        return true;
      }

      if (strcmp(buffer, "false") == 0)
      {
        value_ = false;
        return true;
      }

      return false;
    }

    //--------------------------------------------------------------------------
    template <>
    const char* CVar<bool>::Usage() const
    {
      return "Boolean (case insensitive): true | false";
    }

    //--------------------------------------------------------------------------
    template <>
    bool CVar<double>::ParseFrom(const char* value)
    {
      char* result;
      double v = strtod(value, &result);

      if ((*result) != 0)
      {
        return false;
      }

      bool in_range = true;

      if (range_.has_min == true && v < value_)
      {
        in_range = false;
      }

      if (range_.has_max == true && v > value_)
      {
        in_range = false;
      }

      if (in_range == false)
      {
        Debug::LogVerbosity<1>(
          foundation::LogSeverity::kWarning,
          "The specified value was not in the range for CVar '{0}'\n\
          \n\
          \tThe valid range is: {1} .. {2}",
          name(),
          range_.has_min == true ? "x" : 
          foundation::StringUtils::ToString(range_.min),
          range_.has_max == true ? "x" : 
          foundation::StringUtils::ToString(range_.max));

        return false;
      }

      value_ = v;

      return true;
    }

    //--------------------------------------------------------------------------
    template <>
    const char* CVar<double>::Usage() const
    {
      return 
        "Number: 0x* | * | *.** | *e* | *e-*";
    }

    //--------------------------------------------------------------------------
    template <>
    bool CVar<foundation::String>::ParseFrom(const char* value)
    {
      value_ = value;
      return true;
    }

    //--------------------------------------------------------------------------
    template <>
    const char* CVar<foundation::String>::Usage() const
    {
      return "String: This should always work";
    }
  }
}