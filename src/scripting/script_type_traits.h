#pragma once

#include <EASTL/type_traits.h>
#include <foundation/memory/memory.h>

namespace snuffbox
{
  namespace scripting
  {
    class ScriptValue;
    class ScriptNull;
    class ScriptBoolean;
    class ScriptNumber;
    class ScriptString;
    class ScriptObject;
    class ScriptArray;

    /**
    * @brief Used to only enable a function if the template parameter is a
    *        number, but not a boolean, nor an enum
    *
    * @tparam T The type to check
    */
    template <typename T>
    using if_number = 
      typename eastl::enable_if<
      eastl::is_arithmetic<T>::value == true && 
      eastl::is_enum<T>::value == false &&
      eastl::is_same<T, bool>::value == false, T>::type;

    /**
    * @brief Used to only enable a function if the template parameter is an
    *        enum, but not a boolean, nor a number
    *
    * @tparam T The type to check
    */
    template <typename T>
    using if_enum = 
      typename eastl::enable_if<
      eastl::is_arithmetic<T>::value == false &&
      eastl::is_enum<T>::value == true &&
      eastl::is_same<T, bool>::value == false, T>::type;

    /**
    * @brief Used to only enable a function if the template parameter is both
    *        not a number and not an enumerator, but it can be a boolean
    *
    * @tparam T The type to check
    */
    template <typename T>
    using if_n_number_and_enum =
      typename eastl::enable_if<
      (eastl::is_arithmetic<T>::value == false &&
      eastl::is_enum<T>::value == false) ||
      eastl::is_same<T, bool>::value == true,
      T>::type;

    /**
    * @brief A type trait to check whether a type is derived from a ScriptValue
    *        and stored in a SharedPtr
    *
    * @tparam T The type to check
    *
    * @author Daniel Konings
    */
    template <typename T>
    struct is_script_handle : eastl::false_type {};

    /**
    * @see is_script_handle
    *
    * @brief Specialization for ScriptValue
    */
    template <>
    struct is_script_handle<foundation::SharedPtr<ScriptValue>> 
      : eastl::true_type {};
    
    /**
    * @see is_script_handle
    *
    * @brief Specialization for ScriptNull
    */
    template <>
    struct is_script_handle<foundation::SharedPtr<ScriptNull>> 
      : eastl::true_type {};
    
    /**
    * @see is_script_handle
    *
    * @brief Specialization for ScriptBoolean
    */
    template <>
    struct is_script_handle<foundation::SharedPtr<ScriptBoolean>> 
      : eastl::true_type {};
    
    /**
    * @see is_script_handle
    *
    * @brief Specialization for ScriptNumber
    */
    template <>
    struct is_script_handle<foundation::SharedPtr<ScriptNumber>> 
      : eastl::true_type {};
    
    /**
    * @see is_script_handle
    *
    * @brief Specialization for ScriptString
    */
    template <>
    struct is_script_handle<foundation::SharedPtr<ScriptString>> 
      : eastl::true_type {};
    
    /**
    * @see is_script_handle
    *
    * @brief Specialization for ScriptObject
    */
    template <>
    struct is_script_handle<foundation::SharedPtr<ScriptObject>> 
      : eastl::true_type {};

    /**
    * @see is_script_handle 
    *
    * @brief Specialization for ScriptArray
    */
    template <>
    struct is_script_handle<foundation::SharedPtr<ScriptArray>> 
      : eastl::true_type {};

    /**
    * @brief Used to enable functions based on if the template type is
    *        a ScriptHandle
    *
    * @tparam T The type to check
    */
    template <typename T>
    using if_script_handle =
      typename eastl::enable_if<is_script_handle<T>::value == true, T>::type;

    /**
    * @brief Used to enable functions based on if the template type is
    *        not a ScriptHandle
    *
    * @tparam T The type to check
    */
    template <typename T>
    using if_n_script_handle =
      typename eastl::enable_if<is_script_handle<T>::value == false, T>::type;
  }
}