#pragma once

#include <foundation/containers/function.h>
#include <foundation/containers/string.h>
#include <foundation/containers/uuid.h>
#include <foundation/containers/vector.h>
#include <foundation/memory/memory.h>

#include <engine/assets/asset.h>

#include <glm/glm.hpp>

#include <EASTL/type_traits.h>

namespace snuffbox
{
  namespace editor
  {
    /**
    * @brief The base class for every property to use
    * 
    * Properties are an abstract data type that can be used to apply and
    * revert properties of specific entities. This can be done from their
    * name value and the corresponding property type.
    *
    * A getter and setter can be defined per property, so that we can
    * create specific data-bindings.
    */
    class PropertyValue
    {

    public:

      virtual bool IsNumber() const;
      virtual bool IsString() const;
      virtual bool IsUUID() const;
      virtual bool IsVector(int length) const;
      virtual bool IsAsset() const;
      virtual bool IsList() const;
      virtual bool IsUserData() const;
    };

    template <typename T, typename Y>
    class Property : public PropertyValue
    {

    public:

      typedef void(*Setter)(Y*, const T&);
      typedef T(*Getter)(Y*);

      Property() = delete;
      Property(const T& default_value);

      bool IsNumber() const override;
      bool IsString() const override;
      bool IsUUID() const override;
      bool IsVector(int length) const override;
      bool IsAsset() const override;
      bool IsList() const override;
      bool IsUserData() const override;
    };

    class PropertyList : public PropertyValue
    {

    public:

      PropertyList() = delete;
      PropertyList(const foundation::String& name);

      bool IsList() const override;

      void Add(PropertyValue* value);
      void Insert(int index, PropertyValue* value);
      void Remove(PropertyValue* value);
      void RemoveAt(int index);

      size_t IndexOf(PropertyValue* value);
      foundation::SharedPtr<PropertyValue> At(int index)const;
      size_t Size() const;

      void Clear();

    private:

      foundation::Vector<foundation::SharedPtr<PropertyValue>> values_;
    };

    //--------------------------------------------------------------------------
    template <typename T, typename Y>
    inline bool Property<T, Y>::IsUserData() const
    {
      return 
        IsNumber() == false && 
        IsString() == false && 
        IsUUID() == false && 
        IsVector(2) == false &&
        IsVector(3) == false &&
        IsVector(4) == false &&
        IsAsset() == false &&
        IsList() == false;
    }

    //--------------------------------------------------------------------------
    template <typename T, typename Y>
    inline bool Property<T, Y>::IsNumber() const
    {
      return eastl::is_same<double, T>::value;
    }

    //--------------------------------------------------------------------------
    template <typename T, typename Y>
    inline bool Property<T, Y>::IsString() const
    {
      return eastl::is_same<foundation::String, T>::value;
    }

    //--------------------------------------------------------------------------
    template <typename T, typename Y>
    inline bool Property<T, Y>::IsUUID() const
    {
      return eastl::is_same<foundation::UUID, T>::value;
    }

    //--------------------------------------------------------------------------
    template <typename T, typename Y>
    inline bool Property<T, Y>::IsVector(int length) const
    {
      if (length == 2)
      {
        return eastl::is_same<glm::vec2, T>::value;
      }
      else if (length == 3)
      {
        return eastl::is_same<glm::vec3, T>::value;
      }
      else if (length == 4)
      {
        return eastl::is_same<glm::vec4, T>::value;
      }

      return false;
    }

    //--------------------------------------------------------------------------
    template <typename T, typename Y>
    inline bool Property<T, Y>::IsAsset() const
    {
      return eastl::is_same<engine::SerializableAsset, T>::value;
    }
  }
}