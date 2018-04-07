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
  }
}