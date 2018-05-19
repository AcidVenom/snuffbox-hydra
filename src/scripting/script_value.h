#pragma once

#include "scripting/script_type_traits.h"
#include "scripting/definitions/script_types.h"

#include <foundation/containers/string.h>
#include <foundation/containers/vector.h>
#include <foundation/containers/map.h>

#include <foundation/memory/memory.h>

#include <glm/glm.hpp>

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
    * @brief A short-hand for an array value stored in a SharedPtr
    */
    using ScriptArrayHandle = foundation::SharedPtr<ScriptArray>;
    
    /**
    * @brief A short-hand for an object value stored in a SharedPtr
    */
    using ScriptObjectHandle = foundation::SharedPtr<ScriptObject>;

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
      * @brief Converts a type enumerator value to a string
      *
      * @param[in] type The type to convert
      *
      * @return The converted string
      */
      static const char* TypeToString(ScriptValueTypes type);

      /**
      * @brief Construct by specifying the type
      *
      * @param[in] type The type of this value
      */
      ScriptValue(ScriptValueTypes type);

      /**
      * @brief Used to cast any number value
      *
      * @see ScriptValue::FromImpl
      */
      template <typename T>
      static ScriptHandle From(T value, if_number<T>* = nullptr);

      /**
      * @brief Used to cast any enum value
      *
      * @see ScriptValue::FromImpl
      */
      template <typename T>
      static ScriptHandle From(T value, if_enum<T>* = nullptr);

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
      ScriptValueTypes type() const;


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
      static ScriptHandle FromImpl(T value, if_script_handle<T>* = nullptr);

      /**
      * @brief Constructs a script value by another script value
      *
      * @see ScriptValue::FromImpl
      */
      template <typename T>
      static ScriptHandle FromImpl(T value, if_n_script_handle<T>* = nullptr);

      /**
      * @brief Constructs a script value from a GLM vector
      *
      * @tparam T The glm vector type
      *
      * @return The converted script handle
      */
      template <typename T>
      static ScriptHandle FromVector(T value);

    private:

      ScriptValueTypes type_; //!< The type of this value
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

      /**
      * @brief Checks if a key exists in this object
      *
      * @param[in] key The key to check for
      *
      * @return Does the key exist in this object?
      */
      bool Contains(const char* key) const;

      /**
      * @brief Retrieves a value from this object
      *
      * @param[in] key The key to retrieve the value from
      *
      * @return The found value, or nullptr if it doesn't exist
      */
      ScriptValue* Get(const char* key) const;

      /**
      * @brief Inserts a value at a provided key
      *
      * This converts the type to its respective handle using ScriptValue::From.
      * If the value is a ScriptHandle, it will just be inserted into the map.
      *
      * @tparam T The type of the value to insert
      *
      * @param[in] key The key to insert the value at
      * @param[in] value The value to insert
      */
      template <typename T>
      void Insert(const char* key, T value);

      /**
      * @brief Sets the underlying pointer of this object
      *
      * This pointer is to be set by the ScriptState when a ScriptObject
      * is requested that has a hidden pointer field
      *
      * The type should be set accordingly as well, so that we can check
      * whether a pointer is actually valid for a requested type.
      *
      * @param[in] ptr The pointer to set
      * @param[in] type The type name to set
      */
      void SetPointer(void* ptr, const char* type);

      /**
      * @brief Retrieves the underlying pointer
      *
      * @param[in] type The type that we expect as a named string
      *
      * @return The pointer, or nullptr if there was a type mismatch
      */
      void* GetPointer(const char* type) const;

      /**
      * @return The underlying pointer for userdata objects
      */
      void* ptr() const;

      /**
      * @return The type of the underlying pointer
      */
      const foundation::String& ptr_type() const;

    private:

      void* ptr_; //!< An underlying pointer for userdata objects
      foundation::String ptr_type_; //!< The type of the underlying pointer
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

      /**
      * @brief Adds a value to the array
      *
      * This converts the type to its respective handle using ScriptValue::From.
      * If the value is a ScriptHandle, it will just be inserted into the list.
      *
      * @tparam T The type of the value to insert
      *
      * @param[in] value The value to insert
      */
      template <typename T>
      void Add(T value);
    };

    //--------------------------------------------------------------------------
    template <typename T>
    inline ScriptHandle ScriptValue::From(T value, if_number<T>*)
    {
      return FromImpl<double>(static_cast<double>(value));
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline ScriptHandle ScriptValue::From(T value, if_enum<T>*)
    {
      return From<int>(static_cast<int>(value));
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline ScriptHandle ScriptValue::From(T value, if_n_number_and_enum<T>*)
    {
      return FromImpl<T>(value);
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline ScriptHandle ScriptValue::FromImpl(
      T value,
      if_script_handle<T>*)
    {
      return eastl::static_pointer_cast<ScriptValue>(value);
    }

    //--------------------------------------------------------------------------
    template <>
    inline void ScriptObject::Insert(const char* key, ScriptHandle value)
    {
      emplace(eastl::pair<foundation::String, ScriptHandle>
      {
        foundation::String(key), value
      });
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void ScriptObject::Insert(const char* key, T value)
    {
      ScriptHandle handle = ScriptValue::From<T>(value);
      Insert(key, handle);
    }

    //--------------------------------------------------------------------------
    template <>
    inline void ScriptArray::Add(ScriptHandle value)
    {
      push_back(value);
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void ScriptArray::Add(T value)
    {
      ScriptHandle handle = ScriptValue::From<T>(value);
      Add(handle);
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline ScriptHandle ScriptValue::FromVector(T value)
    {
      static const char* keys[] =
      {
        "x", "y", "z", "w"
      };

      static_assert(T::length() <= 4, 
        "Only vectors with a component count of\
        <= 4 can be converted to a ScriptValue");

      foundation::SharedPtr<ScriptObject> obj = CreateObject();

      int n = T::length();

      for (int i = 0; i < n; ++i)
      {
        obj->Insert(keys[i], value[i]);
      }

      return obj;
    }
  }
}