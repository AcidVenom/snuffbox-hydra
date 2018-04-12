#pragma once

#include "scripting/script_type_traits.h"

#include <foundation/containers/string.h>
#include <foundation/containers/vector.h>
#include <foundation/containers/map.h>

#include <foundation/memory/memory.h>

namespace snuffbox
{
  namespace scripting
  {
    class ScriptValue;
    class ScriptObject;
    class ScriptArray;

    /**
    * @brief A short-hand for all the values stored in a SharedPtr
    */
    using ScriptHandle = foundation::SharedPtr<ScriptValue>;

    /**
    * @brief Used to store values from the scripting environment as C++
    *        data types
    *
    * This should serve as the base class for every scripting value to use.
    * The values should be retrieved via the script state, where the native
    * implementation makes sure the appropriate value is returned.
    *
    * @author Daniel Konings
    */
    class ScriptValue
    {

    public:

      /**
      * @brief The different type of values that exist
      */
      enum struct Types
      {
        kNull, //!< Null/undefined value
        kBoolean, //!< Boolean values
        kNumber, //!< Numerical values, they are always double
        kString, //!< String values stored as an EASTL string
        kObject, //!< Object values by key/value pairs
        kArray, //!< Array values by indices,
        kUserdata //!< A userdata value
      };

      /**
      * @brief Construct by specifying the type
      *
      * @param[in] type The type of this value
      */
      ScriptValue(Types type);

      /**
      * @brief Used to cast any number or enumerator value
      *
      * @see ScriptValue::FromImpl
      */
      template <typename T>
      static ScriptHandle From(T value, if_number_or_enum<T>* = nullptr);

      /**
      * @brief Used to cast any other value
      *
      * @see ScriptValue::FromImpl
      */
      template <typename T>
      static ScriptHandle From(T value, if_n_number_and_enum<T>* = nullptr);

      /**
      * @brief Creates a scriptable object
      *
      * @return The created object
      */
      static foundation::SharedPtr<ScriptObject> CreateObject();

      /**
      * @brief Creates a scriptable array
      *
      * @return The created array
      */
      static foundation::SharedPtr<ScriptArray> CreateArray();

      /**
      * @return The type of this value
      */
      Types type() const;


    protected:

      /**
      * @brief Constructs a script value from a type
      *
      * @tparam T The type to convert
      *
      * @param[in] value The value to store in the converted value
      *
      * @return The converted value
      */
      template <typename T>
      static ScriptHandle FromImpl(T value);

    private:

      Types type_; //!< The type of this value
    };

    /**
    * @brief A "null", "nil" or "undefined" data type
    *
    * This class is basically empty, as there is simply no functionality
    * for empty objects
    *
    * @author Daniel Konings
    */
    class ScriptNull : public ScriptValue
    {

    public:

      /**
      * @brief Default constructor, simply assigns the type
      */
      ScriptNull();
    };

    /**
    * @brief A boolean value
    *
    * Booleans are a bit iffy, as some scripting languages treat many of their
    * types as booleans as well. The native scripting implementation should take
    * care of finding the right type to use. (As most types are castable to a
    * boolean)
    *
    * @author Daniel Konings
    */
    class ScriptBoolean : public ScriptValue
    {

    public:

      /**
      * @brief Construct from a boolean value
      *
      * @remarks Assigns the type for the ScriptValue base
      */
      ScriptBoolean(bool value);

      /**
      * @return The value of this script boolean
      */
      bool value() const;

    private:

      bool value_; //!< The value of this script boolean
    };

    /**
    * @brief A script number, always a double
    *
    * @author Daniel Konings
    */
    class ScriptNumber : public ScriptValue
    {

    public:

      /**
      * @brief Construct from a double value
      *
      * @remarks Assigns the type for the ScriptValue base
      */
      ScriptNumber(double value);

      /**
      * @return The value of this script number
      */
      double value() const;

    private:

      double value_; //!< The value of this script number
    };

    /**
    * @brief A scripting string
    *
    * @author Daniel Konings
    */
    class ScriptString : public ScriptValue
    {

    public:

      /**
      * @brief Construct from a const char* value
      *
      * @remarks Assigns the type for the ScriptValue base
      */
      ScriptString(const char* value);

      /**
      * @return The value of this script string
      */
      const foundation::String& value() const;

    private:

      foundation::String value_; //!< The value of this script string
    };

    /**
    * @brief A scripting object
    *
    * As this is just a data container, it inherits from its appropriate
    * container, a Map<String, ScriptHandle>.
    *
    * It should be noted these objects should never reference themselves, as
    * making a deep copy would then be very difficult.
    *
    * @author Daniel Konings
    */
    class ScriptObject : 
      public ScriptValue, 
      public foundation::Map<foundation::String, ScriptHandle>
    {

    public:

      /**
      * @brief Default constructor, simply assigns the type
      */
      ScriptObject();
    };

    /**
    * @brief A scripting array
    *
    * Like the ScriptObject; this inherits from its appropriate container,
    * a Vector<String, ScriptHandle>. 
    *
    * Arrays are often not really arrays in scripting languages.
    * For instance; in both JavaScript and Lua, they're just indexed by a
    * stringified version of their index value. However, here we store them
    * as actual arrays.
    *
    * It should be noted these objects should never reference themselves, as
    * making a deep copy would then be very difficult.
    *
    * @author Daniel Konings
    */
    class ScriptArray : 
      public ScriptValue, 
      public foundation::Vector<ScriptHandle>
    {

    public:

      /**
      * @brief Default constructor, simply assigns the type
      */
      ScriptArray();
    };

    /**
    * @brief A userdata pointer
    *
    * @tparam T The expected pointer type
    *
    * This data type should also contain a reference to its corresponding object
    *
    * The userdata should be checked by the native scripting API, to make sure
    * that the pointer stored is of the actual type the user expects
    *
    * @see ScriptObject
    *
    * @author Daniel Konings
    */
    template <typename T>
    class ScriptUserdata : public ScriptValue
    {

    public:

      /**
      * @brief Construct from both the pointer and the corresponding object
      *
      * @param[in] ptr The pointer to store in this userdata
      * @param[in] object The object handle to store
      */
      ScriptUserdata(T* ptr, foundation::SharedPtr<ScriptObject> object);

      /**
      * @return The stored pointer
      */
      T* ptr() const;

      /**
      * @return The object this userdata corresponds to
      */
      foundation::SharedPtr<ScriptObject> object() const;

    private:

      T* ptr_; //!< The stored pointer

      /**
      * @brief The object this userdata corresponds to
      */
      foundation::SharedPtr<ScriptObject> object_;
    };

    //--------------------------------------------------------------------------
    template <typename T>
    inline ScriptHandle ScriptValue::From(T value, if_number_or_enum<T>*)
    {
      return static_cast<T>(FromImpl<double>(value));
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline ScriptHandle ScriptValue::From(T value, if_n_number_and_enum<T>*)
    {
      return static_cast<T>(value);
    }

    //--------------------------------------------------------------------------
    template <>
    inline ScriptHandle ScriptValue::FromImpl(double value)
    {
      return foundation::Memory::ConstructShared<ScriptNumber>(
        &foundation::Memory::default_allocator(),
        value
        );
    }

    //--------------------------------------------------------------------------
    template <>
    inline ScriptHandle ScriptValue::FromImpl(bool value)
    {
      return foundation::Memory::ConstructShared<ScriptBoolean>(
        &foundation::Memory::default_allocator(),
        value
        );
    }

    //--------------------------------------------------------------------------
    template <>
    inline ScriptHandle ScriptValue::FromImpl(const char* value)
    {
      return foundation::Memory::ConstructShared<ScriptString>(
        &foundation::Memory::default_allocator(),
        value
        );
    }

    //--------------------------------------------------------------------------
    template <>
    inline ScriptHandle ScriptValue::FromImpl(foundation::String value)
    {
      return foundation::Memory::ConstructShared<ScriptString>(
        &foundation::Memory::default_allocator(),
        value.c_str()
        );
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline ScriptUserdata<T>::ScriptUserdata(
      T* ptr,
      foundation::SharedPtr<ScriptObject> object)
      :
      ScriptValue(ScriptValue::Types::kUserdata)
      ptr_(ptr),
      object_(object)
    {

    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline T* ScriptUserdata<T>::ptr() const
    {
      return ptr_;
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline foundation::SharedPtr<ScriptObject> ScriptUserdata<T>::object() const
    {
      return object_;
    }
  }
}