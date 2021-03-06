#pragma once

#include <foundation/containers/function.h>
#include <foundation/containers/string.h>
#include <foundation/containers/uuid.h>
#include <foundation/containers/vector.h>
#include <foundation/containers/map.h>
#include <foundation/memory/memory.h>

#include <engine/assets/asset.h>

#include <glm/glm.hpp>

#include <EASTL/type_traits.h>
#include <cinttypes>

namespace snuffbox
{
  namespace editor
  {
    /**
    * @brief Enum properties should always be int32_t
    */
    using EnumProperty = int32_t;

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

      /**
      * @return Is this property a boolean type?
      */
      virtual bool IsBoolean() const;

      /**
      * @return Is this property a number type?
      */
      virtual bool IsNumber() const;

      /**
      * @return Is this property a string type?
      */
      virtual bool IsString() const;

      /**
      * @return Is this property a UUID type?
      */
      virtual bool IsUUID() const;

      /**
      * @brief Checks if this property is a specific vector type
      *
      * @param[in] length The length to check for
      *
      * @return Is this property a Vec2, Vec3 or Vec4 type?
      */
      virtual bool IsVector(int length) const;

      /**
      * @return Is this property an enumerator type?
      */
      virtual bool IsEnum() const;

      /**
      * @return Is this property an asset type?
      */
      virtual bool IsAsset() const;

      /**
      * @brief Sets a boolean value
      *
      * @param[in] object The object to operate on
      * @param[in] value The new value
      */
      virtual void Set(void* object, const bool& value);

      /**
      * @brief Sets a number value
      *
      * @param[in] object The object to operate on
      * @param[in] value The new value
      */
      virtual void Set(void* object, const double& value);

      /**
      * @brief Sets a string value
      *
      * @param[in] object The object to operate on
      * @param[in] value The new value
      */
      virtual void Set(void* object, const foundation::String& value);

      /**
      * @brief Sets a UUID value
      *
      * @param[in] object The object to operate on
      * @param[in] value The new value
      */
      virtual void Set(void* object, const foundation::UUID& value);

      /**
      * @brief Sets a Vec2 value
      *
      * @param[in] object The object to operate on
      * @param[in] value The new value
      */
      virtual void Set(void* object, const glm::vec2& value);

      /**
      * @brief Sets a Vec3 value
      *
      * @param[in] object The object to operate on
      * @param[in] value The new value
      */
      virtual void Set(void* object, const glm::vec3& value);

      /**
      * @brief Sets a Vec4 value
      *
      * @param[in] object The object to operate on
      * @param[in] value The new value
      */
      virtual void Set(void* object, const glm::vec4& value);

      /**
      * @brief Sets an enumerator value
      *
      * @param[in] object The object to operate on
      * @param[in] value The new value
      */
      virtual void Set(void* object, const EnumProperty& value);

      /**
      * @brief Sets an asset value
      *
      * @param[in] object The object to operate on
      * @param[in] value The new value
      */
      virtual void Set(void* object, const engine::SerializableAsset& value);

      /**
      * @brief Gets a contained value as raw data
      *
      * @param[in] opject The object to retrieve the value from
      * @param[in] buffer The buffer to write to, or nullptr if only
      *                   checking for size
      *
      * @param[in|out] required The required size of the buffer. This will
      *                         contain the required size of the buffer in the
      *                         case that nullptr has been passed for 
      *                         that buffer, or used as a reference for filling
      *                         the buffer when a non-null buffer is provided.
      *
      * @return Was this a valid operation?
      */
      virtual bool GetRaw(
        void* object, 
        uint8_t* buffer, 
        size_t* required) const = 0;

      /**
      * @brief Sets a value by combo box index
      *
      * @remarks This function will only work for enumerator properties
      *
      * @param[in] object The object to operate on
      * @param[in] combo_idx The index of the combo box values to set
      */
      virtual void SetComboValue(void* object, int combo_idx);

      /**
      * @brief We can set a list of combo box values so that we can set values
      *        on this property by name. This is currently mostly useful
      *        for enumerator values.
      *
      * @param[in] values The values to set
      */
      void set_combo_box_values(
        const foundation::Vector<foundation::String>& values);

      /**
      * @return The list of combo box values
      */
      const foundation::Vector<foundation::String>& combo_box_values() const;

    private:

      /**
      * @brief The list of combo box values
      */
      foundation::Vector<foundation::String> combo_box_values_;
    };

    /**
    * @brief Used to build a map of properties that we can update by name
    */
    using PropertyMap =
      foundation::Map<foundation::String, foundation::SharedPtr<PropertyValue>>;

    /**
    * @brief Used to insert items into the property map
    */
    using PropertyPair =
      eastl::pair<foundation::String, foundation::SharedPtr<PropertyValue>>;

    /**
    * @brief A property strongly typed property value, with both the
    *        type of the property and the type of the object to operate on
    *
    * @tparam T The type of the property
    * @tparam Y The type of the object to operate on
    *
    * @author Daniel Konings
    */
    template <typename T, typename Y>
    class Property : public PropertyValue
    {

    public:

      /**
      * @brief A type definition for the setter data binding
      */
      typedef void(*Setter)(Y*, const T&);

      /**
      * @brief A type definition for the getter data binding
      */
      typedef T(*Getter)(Y*);

      /**
      * @brief Delete default constructor
      */
      Property() = delete;

      /**
      * @brief Construct through a setter binding and a getter binding
      *
      * @param[in] setter The setter binding
      * @param[in] getter The getter binding
      */
      Property(Setter setter, Getter getter);

      /**
      * @see PropertyValue::IsBoolean
      */
      bool IsBoolean() const override;

      /**
      * @see PropertyValue::IsNumber
      */
      bool IsNumber() const override;

      /**
      * @see PropertyValue::IsString
      */
      bool IsString() const override;

      /**
      * @see PropertyValue::IsUUID
      */
      bool IsUUID() const override;

      /**
      * @see PropertyValue::IsVector
      */
      bool IsVector(int length) const override;

      /**
      * @see PropertyValue::IsEnum
      */
      bool IsEnum() const override;

      /**
      * @see PropertyValue::IsAsset
      */
      bool IsAsset() const override;

      /**
      * @see PropertyValue::Set
      */
      void Set(void* object, const T& value) override;

      /**
      * @see PropertyValue::SetComboValue
      */
      void SetComboValue(void* object, int combo_idx) override;

      /**
      * @see PropertyValue::GetRaw
      */
      bool GetRaw(
        void* object, 
        uint8_t* buffer, 
        size_t* required) const override;
      

      /**
      * @brief Retrieves a value from within the object that is being operated
      *        on, if a getter is defined
      *
      * @remarks This returns a default constructed value if no getter is set
      *
      * @param[in] object The object to operate on
      *
      * @return The retrieved value
      */
      T Get(Y* object) const;

    private:

      Setter setter_; //!< The setter binding
      Getter getter_; //!< The getter binding
    };

    /**
    * @brief Used to cast abstract, shared, properties to their respective
    *        typed property
    *
    * @tparam T The expected property type
    * @tparam Y The expected object type the property operates on
    *
    * @param[in] value The abstract property to cast
    *
    * @return The casted property
    */
    template <typename T, typename Y>
    foundation::SharedPtr<Property<T, Y>> PropertyCast(
      const foundation::SharedPtr<PropertyValue>& value)
    {
      return eastl::static_shared_pointer_cast<Property<T, Y>, PropertyValue>(
        value);
    }

    /**
    * @brief A short hand to create a property, because else it could get
    *        a bit messy in-code..
    *
    * @tparam T The type of the property
    * @tparam Y The tpye of the object to operate on
    *
    * Self explanatory, simply calls ConstructShared to create a new shared
    * pointer of a typed property
    */
    template <typename T, typename Y>
    foundation::SharedPtr<PropertyValue> CreateProperty(
      typename Property<T, Y>::Setter setter, 
      typename Property<T, Y>::Getter getter)
    {
      using P = Property<T, Y>;
      return eastl::static_shared_pointer_cast<PropertyValue, P>(
        foundation::Memory::ConstructShared<P>(
          &foundation::Memory::default_allocator(),
          setter,
          getter));
    }

    /**
    * @brief A short hand to create a property pair to add to a property map
    *
    * @tparam T The type of the property
    * @tparam Y The type of the object to operate on
    *
    * @see CreateProperty
    */
    template <typename T, typename Y>
    PropertyPair CreatePropertyPair(
      const foundation::String& name,
      typename Property<T, Y>::Setter setter,
      typename Property<T, Y>::Getter getter)
    {
      return PropertyPair(name, CreateProperty<T, Y>(setter, getter));
    }

    //--------------------------------------------------------------------------

    /**
    * @brief Used to copy property files by raw data
    *
    * Usually we can just memcpy our data over, however;
    * this can be different for different property types. Take for instance
    * a string, its container can be located elsewhere, perhaps it doesn't
    * have contiguous memory. This struct provides helper functionality for
    * such classes utilizing SFINAE.
    *
    * @author Daniel Konings
    */
    template <typename T>
    struct PropertyDataCopy
    {

      //------------------------------------------------------------------------

      /**
      * @brief String copy
      */
      template <typename Q = T>
      static typename eastl::enable_if<
        eastl::is_same<Q, foundation::String>::value, void>::type 
        Copy(
          uint8_t* buffer, 
          const T& value, 
          size_t size)
      {
        memcpy(buffer, value.c_str(), size);
        memset(buffer + size, '\0', 1);
      }

      //------------------------------------------------------------------------

      /**
      * @brief Regular copy
      */
      template <typename Q = T>
      static typename eastl::enable_if<
        !eastl::is_same<Q, foundation::String>::value, void>::type 
        Copy(
          uint8_t* buffer,
          const T& value,
          size_t size)
      {
        memcpy(buffer, &value, size);
      }

      //------------------------------------------------------------------------

      /**
      * @brief String size
      */
      template <typename Q = T>
      static typename eastl::enable_if<
        eastl::is_same<Q, foundation::String>::value, size_t>::type 
        Required(const T& value)
      {
        return value.size() + 1;
      }

      //------------------------------------------------------------------------

      /**
      * @brief Regular size
      */
      template <typename Q = T>
      static typename eastl::enable_if<
        !eastl::is_same<Q, foundation::String>::value, size_t>::type
        Required(const T& value)
      {
        return sizeof(value);
      }
    };

    //--------------------------------------------------------------------------

    /**
    * @brief Used to set combo box values, but only for enum properties
    *
    * @author Daniel Konings
    */
    template <typename T, typename Y>
    struct PropertyComboBox
    {
      //------------------------------------------------------------------------

      /**
      * @brief Enum value
      */
      template <typename Q = T>
      inline static typename eastl::enable_if<
        eastl::is_same<Q, EnumProperty>::value, void>::type
        Set(Y* object, int idx, typename Property<T, Y>::Setter setter)
      {
        if (setter == nullptr)
        {
          return;
        }

        setter(object, static_cast<EnumProperty>(idx));
      }

      //------------------------------------------------------------------------

      /**
      * @brief Non-enum value
      */
      template <typename Q = T>
      inline static typename eastl::enable_if<
        !eastl::is_same<Q, EnumProperty>::value, void>::type
        Set(Y* object, int idx, typename Property<T, Y>::Setter setter)
      {
        foundation::Logger::Assert(false,
          "Attempted to set a combo box value on a non-enum property");
      }
    };

    //--------------------------------------------------------------------------
    template <typename T, typename Y>
    inline Property<T, Y>::Property(Setter setter, Getter getter) :
      setter_(setter),
      getter_(getter)
    {

    }

    //--------------------------------------------------------------------------
    template <typename T, typename Y>
    inline bool Property<T, Y>::IsBoolean() const
    {
      return eastl::is_same<bool, T>::value;
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
    inline bool Property<T, Y>::IsEnum() const
    {
      return eastl::is_same<EnumProperty, T>::value;
    }

    //--------------------------------------------------------------------------
    template <typename T, typename Y>
    inline bool Property<T, Y>::IsAsset() const
    {
      return eastl::is_same<engine::SerializableAsset, T>::value;
    }

    //--------------------------------------------------------------------------
    template <typename T, typename Y>
    inline void Property<T, Y>::Set(void* object, const T& value)
    {
      if (setter_ == nullptr)
      {
        return;
      }

      Y* ptr = reinterpret_cast<Y*>(object);
      setter_(ptr, value);
    }

    //--------------------------------------------------------------------------
    template <typename T, typename Y>
    inline void Property<T, Y>::SetComboValue(void* object, int combo_idx)
    {
      if (IsEnum() == false)
      {
        return;
      }

      if (combo_idx < 0 || combo_idx >= combo_box_values().size())
      {
        return;
      }

      Y* ptr = reinterpret_cast<Y*>(object);
      PropertyComboBox<T, Y>::Set(ptr, combo_idx, setter_);
    }

    //--------------------------------------------------------------------------
    template <typename T, typename Y>
    inline bool Property<T, Y>::GetRaw(
      void* object, 
      uint8_t* buffer, 
      size_t* required) const
    {
      size_t req = 0;
      if (object != nullptr)
      {
        T value = Get(reinterpret_cast<Y*>(object));

        if (buffer != nullptr && required != nullptr)
        {
          PropertyDataCopy<T>::Copy(buffer, value, *required);
          req = *required;
        }
        else
        {
          req = PropertyDataCopy<T>::Required(value);
        }
      }

      if (required != nullptr)
      {
        *required = req;
      }

      return req != 0;
    }

    //--------------------------------------------------------------------------
    template <typename T, typename Y>
    inline T Property<T, Y>::Get(Y* object) const
    {
      if (getter_ == nullptr)
      {
        return T();
      }

      return getter_(object);
    }
  }
}