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
    bool PropertyValue::IsAsset() const
    {
      return false;
    }

    //--------------------------------------------------------------------------
    bool PropertyValue::IsList() const
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
    void PropertyValue::Set(
      void* object, 
      const engine::SerializableAsset& value)
    {
      foundation::Logger::Assert(IsAsset() == true,
        "Attempted to set an asset value on a non-asset property");
    }

    //--------------------------------------------------------------------------
    PropertyList::PropertyList() :
      was_changed_(false)
    {

    }

    //--------------------------------------------------------------------------
    bool PropertyList::IsList() const
    {
      return true;
    }

    //--------------------------------------------------------------------------
    bool PropertyList::HasChanged(void* object)
    {
      bool changed = false;
      if (was_changed_ == false)
      {
        for (size_t i = 0; i < values_.size(); ++i)
        {
          if (values_.at(i)->HasChanged(object) == true)
          {
            changed = true;
            break;
          }
        }
      }
      else
      {
        was_changed_ = false;
        changed = true;
      }

      return changed;
    }

    //--------------------------------------------------------------------------
    void PropertyList::Add(const foundation::SharedPtr<PropertyValue>& value)
    {
      values_.push_back(value);
      was_changed_ = true;
    }

    //--------------------------------------------------------------------------
    void PropertyList::Insert(
      int index, 
      const foundation::SharedPtr<PropertyValue>& value)
    {
      values_.insert(values_.begin() + index, value);
      was_changed_ = true;
    }

    //--------------------------------------------------------------------------
    void PropertyList::Remove(const foundation::SharedPtr<PropertyValue>& value)
    {
      int idx = IndexOf(value);

      if (idx < 0)
      {
        return;
      }

      values_.erase(values_.begin() + idx);
    }

    //--------------------------------------------------------------------------
    void PropertyList::RemoveAt(int index)
    {
      if (index < 0 || index >= Count())
      {
        return;
      }

      values_.erase(values_.begin() + index);
    }

    //--------------------------------------------------------------------------
    int PropertyList::IndexOf(const foundation::SharedPtr<PropertyValue>& value)
    {
      for (int i = 0; i < values_.size(); ++i)
      {
        if (values_.at(i) == value)
        {
          return i;
        }
      }

      return -1;
    }

    //--------------------------------------------------------------------------
    foundation::SharedPtr<PropertyValue> PropertyList::At(int index) const
    {
      if (index < 0 || index >= Count())
      {
        return nullptr;
      }

      return values_.at(index);
    }

    //--------------------------------------------------------------------------
    int PropertyList::Count() const
    {
      return static_cast<int>(values_.size());
    }

    //--------------------------------------------------------------------------
    void PropertyList::Clear()
    {
      values_.clear();
    }

    //--------------------------------------------------------------------------
    PropertyList::~PropertyList()
    {
      Clear();
    }
  }
}