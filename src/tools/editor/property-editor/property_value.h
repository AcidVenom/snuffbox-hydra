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
      * @return Is this property an asset type?
      */
      virtual bool IsAsset() const;

      /**
      * @return Is this property a list of different properties?
      */
      virtual bool IsList() const;

      /**
      * @brief Checks if this specific property within an object has been
      *        changed
      *
      * @param[in] object The object to operate on
      *
      * @return Has this property for that object been changed?
      *
      * @remarks This should automatically update the value within the
      *          property, so that a newer call will evaluate to false
      */
      virtual bool HasChanged(void* object) = 0;

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
      * @brief Sets an asset value
      *
      * @param[in] object The object to operate on
      * @param[in] value The new value
      */
      virtual void Set(void* object, const engine::SerializableAsset& value);
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
      * @see PropertyValue::IsAsset
      */
      bool IsAsset() const override;

      /**
      * @see PropertyValue::HasChanged
      */
      bool HasChanged(void* object) override;

      /**
      * @see PropertyValue::Set
      */
      void Set(void* object, const T& value) override;

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

      T value_; //!< The value contained in this property to check for changes
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
    * @brief Defines a list of properties, which in turn can be used as
    *        a PropertyValue again
    *
    * @remarks All contained values are ref counted using shared pointers
    *
    * @author Daniel Konings
    */
    class PropertyList : public PropertyValue
    {

    public:

      /**
      * @brief Default constructor
      */
      PropertyList();

      /**
      * @see PropertyValue::IsList
      */
      bool IsList() const override;

      /**
      * @see PropertyValue::HasChanged
      */
      bool HasChanged(void* object) override;

      /**
      * @brief Adds a new property value to the list
      *
      * @param[in] value The value to add
      */
      void Add(const foundation::SharedPtr<PropertyValue>& value);

      /**
      * @brief Inserts a new property at a provided index
      *
      * @param[in] index The index to insert at
      * @param[in] value The value to insert
      */
      void Insert(
        int index, 
        const foundation::SharedPtr<PropertyValue>& value);

      /**
      * @brief Removes a property value by value
      *
      * @param[in] value The value to remove
      */
      void Remove(const foundation::SharedPtr<PropertyValue>& value);

      /**
      * @brief Removes a property value at a given index
      *
      * @param[in] index The index of the property value to remove
      */
      void RemoveAt(int index);

      /**
      * @brief Finds the index of a provided property value
      *
      * @param[in] value The value to find
      *
      * @return The index of the item, or -1 if it was not found
      */
      int IndexOf(const foundation::SharedPtr<PropertyValue>& value);

      /**
      * @brief Retrieves a property value at the specified index
      *
      * @param[in] index The index of the property value to retrieve
      *
      * @return The retrieved property value, or nullptr if the index
      *         was out of bounds
      */
      foundation::SharedPtr<PropertyValue> At(int index) const;

      /**
      * @return The number of property values stored within this list
      */
      int Count() const;

      /**
      * @brief Clears all property values in this list, reducing their ref
      *        count by 1
      */
      void Clear();

      /**
      * @brief Clears the entire list
      */
      ~PropertyList();

    private:

      bool was_changed_; //!< Was this list changed?

      /**
      * @brief The entire list of property values
      */
      foundation::Vector<foundation::SharedPtr<PropertyValue>> values_;
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
    inline bool Property<T, Y>::IsAsset() const
    {
      return eastl::is_same<engine::SerializableAsset, T>::value;
    }

    //--------------------------------------------------------------------------
    template <typename T, typename Y>
    inline bool Property<T, Y>::HasChanged(void* object)
    {
      Y* ptr = reinterpret_cast<Y*>(object);
      T new_value = Get(ptr);

      bool changed = value_ == new_value;

      if (changed == true)
      {
        value_ = new_value;
      }

      return changed;
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

      value_ = value;
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