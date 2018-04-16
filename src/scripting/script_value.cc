#include "scripting/script_value.h"

namespace snuffbox
{
  namespace scripting
  {
    //--------------------------------------------------------------------------
    const char* ScriptValue::TypeToString(ScriptValueTypes type)
    {
      switch (type)
      {
      case ScriptValueTypes::kNull:
        return "Null";

      case ScriptValueTypes::kNumber:
        return "Number";

      case ScriptValueTypes::kBoolean:
        return "Boolean";

      case ScriptValueTypes::kString:
        return "String";
        
      case ScriptValueTypes::kObject:
        return "Object";

      case ScriptValueTypes::kArray:
        return "Array";

      default:
        return "Unknown";
      }
    }

    //--------------------------------------------------------------------------
    ScriptValue::ScriptValue(ScriptValueTypes type) :
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
    ScriptValueTypes ScriptValue::type() const
    {
      return type_;
    }

    //--------------------------------------------------------------------------
    ScriptNull::ScriptNull() :
      ScriptValue(ScriptValueTypes::kNull)
    {

    }

    //--------------------------------------------------------------------------
    ScriptBoolean::ScriptBoolean(bool value) :
      ScriptValue(ScriptValueTypes::kBoolean),
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
      ScriptValue(ScriptValueTypes::kNumber),
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
      ScriptValue(ScriptValueTypes::kString),
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
      ScriptValue(ScriptValueTypes::kObject),
      ptr_(nullptr),
      type_("")
    {

    }

    //--------------------------------------------------------------------------
    bool ScriptObject::Contains(const char* key) const
    {
      return find(key) != end();
    }

    //--------------------------------------------------------------------------
    ScriptValue* ScriptObject::Get(const char* key) const
    {
      if (Contains(key) == false)
      {
        return nullptr;
      }
      return find(key)->second.get();
    }

    //--------------------------------------------------------------------------
    void ScriptObject::SetPointer(void* ptr, const char* type)
    {
      ptr_ = ptr;
      type_ = type;
    }

    //--------------------------------------------------------------------------
    void* ScriptObject::GetPointer(const char* type) const
    {
      if (strcmp(type_.c_str(), type) != 0)
      {
        return nullptr;
      }

      return ptr_;
    }

    //--------------------------------------------------------------------------
    ScriptArray::ScriptArray() :
      ScriptValue(ScriptValueTypes::kArray)
    {

    }
  }
}