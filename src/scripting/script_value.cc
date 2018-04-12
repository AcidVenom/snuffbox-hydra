#include "scripting/script_value.h"

namespace snuffbox
{
  namespace scripting
  {
    //--------------------------------------------------------------------------
    ScriptValue::ScriptValue(Types type) :
      type_(type)
    {

    }

    //--------------------------------------------------------------------------
    foundation::SharedPtr<ScriptObject> ScriptValue::CreateObject()
    {
      return foundation::Memory::ConstructShared<ScriptObject>(
        &foundation::Memory::default_allocator()
        );
    }

    //--------------------------------------------------------------------------
    foundation::SharedPtr<ScriptArray> ScriptValue::CreateArray()
    {
      return foundation::Memory::ConstructShared<ScriptArray>(
          &foundation::Memory::default_allocator()
        );
    }

    //--------------------------------------------------------------------------
    ScriptValue::Types ScriptValue::type() const
    {
      return type_;
    }

    //--------------------------------------------------------------------------
    ScriptNull::ScriptNull() :
      ScriptValue(ScriptValue::Types::kNull)
    {

    }

    //--------------------------------------------------------------------------
    ScriptBoolean::ScriptBoolean(bool value) :
      ScriptValue(ScriptValue::Types::kBoolean),
      value_(value)
    {
      
    }

    //--------------------------------------------------------------------------
    bool ScriptBoolean::value() const
    {
      return value_;
    }

    //--------------------------------------------------------------------------
    ScriptNumber::ScriptNumber(double value) :
      ScriptValue(ScriptValue::Types::kNumber),
      value_(value)
    {

    }

    //--------------------------------------------------------------------------
    double ScriptNumber::value() const
    {
      return value_;
    }

    //--------------------------------------------------------------------------
    ScriptString::ScriptString(const char* value) :
      ScriptValue(ScriptValue::Types::kString),
      value_(value)
    {

    }

    //--------------------------------------------------------------------------
    const foundation::String& ScriptString::value() const
    {
      return value_;
    }

    //--------------------------------------------------------------------------
    ScriptObject::ScriptObject() :
      ScriptValue(ScriptValue::Types::kObject)
    {

    }

    //--------------------------------------------------------------------------
    ScriptArray::ScriptArray() :
      ScriptValue(ScriptValue::Types::kArray)
    {

    }
  }
}