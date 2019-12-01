#include "tools/editor/property-editor/property_value.h"

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    bool PropertyValue::IsBoolean() const
    {
      return false;
    }

    //--------------------------------------------------------------------------
    bool PropertyValue::IsNumber() const
    {
      return false;
    }

    //--------------------------------------------------------------------------
    bool PropertyValue::IsString() const
    {
      return false;
    }

    //--------------------------------------------------------------------------
    bool PropertyValue::IsUUID() const
    {
      return false;
    }

    //--------------------------------------------------------------------------
    bool PropertyValue::IsVector(int length) const
    {
      return false;
    }

    //--------------------------------------------------------------------------
    bool PropertyValue::IsEnum() const
    {
      return false;
    }

    //--------------------------------------------------------------------------
    bool PropertyValue::IsAsset() const
    {
      return false;
    }

    //--------------------------------------------------------------------------
    void PropertyValue::Set(void* object, const bool& value)
    {
      foundation::Logger::Assert(IsBoolean() == true,
        "Attempted to set a boolean value on a non-boolean property");
    }

    //--------------------------------------------------------------------------
    void PropertyValue::Set(void* object, const double& value)
    {
      foundation::Logger::Assert(IsNumber() == true, 
        "Attempted to set a number value on a non-number property");
    }

    //--------------------------------------------------------------------------
    void PropertyValue::Set(void* object, const foundation::String& value)
    {
      foundation::Logger::Assert(IsString() == true,
        "Attempted to set a string value on a non-string property");
    }

    //--------------------------------------------------------------------------
    void PropertyValue::Set(void* object, const foundation::UUID& value)
    {
      foundation::Logger::Assert(IsUUID() == true,
        "Attempted to set a UUID value on a non-UUID property");
    }

    //--------------------------------------------------------------------------
    void PropertyValue::Set(void* object, const glm::vec2& value)
    {
      foundation::Logger::Assert(IsVector(2) == true,
        "Attempted to set a vec2 value on a non-vec2 property");
    }

    //--------------------------------------------------------------------------
    void PropertyValue::Set(void* object, const glm::vec3& value)
    {
      foundation::Logger::Assert(IsVector(3) == true,
        "Attempted to set a vec3 value on a non-vec3 property");
    }

    //--------------------------------------------------------------------------
    void PropertyValue::Set(void* object, const glm::vec4& value)
    {
      foundation::Logger::Assert(IsVector(4) == true,
        "Attempted to set a vec4 value on a non-vec4 property");
    }

    //--------------------------------------------------------------------------
    void PropertyValue::Set(void* object, const EnumProperty& value)
    {
      foundation::Logger::Assert(IsEnum() == true,
        "Attempted to set an enum value on a non-enum property");
    }

    //--------------------------------------------------------------------------
    void PropertyValue::Set(
      void* object, 
      const engine::SerializableAsset& value)
    {
      foundation::Logger::Assert(IsAsset() == true,
        "Attempted to set an asset value on a non-asset property");
    }

    //--------------------------------------------------------------------------
    void PropertyValue::SetComboValue(void* object, int combo_idx)
    {
      foundation::Logger::Assert(IsEnum() == true,
        "Attempted to set a combo box value on a non-enum property");
    }

    //--------------------------------------------------------------------------
    void PropertyValue::set_combo_box_values(
      const foundation::Vector<foundation::String>& values)
    {
      combo_box_values_ = values;
    }

    //--------------------------------------------------------------------------
    const foundation::Vector<foundation::String>& 
      PropertyValue::combo_box_values() const
    {
      return combo_box_values_;
    }
  }
}