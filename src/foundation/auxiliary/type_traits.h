#pragma once

#include "foundation/containers/vector.h"

#include <EASTL/type_traits.h>
#include <type_traits>

namespace snuffbox
{
  namespace foundation
  {
    /**
    * @brief Used to check if a value type is a number
    *
    * @tparam T The type to check
    *
    * @author Daniel Konings
    */
    template <typename T>
    struct is_number
    {
      /**
      * @brief Is T a number?
      */
      static const bool value = 
        eastl::is_arithmetic<T>::value == true && 
        eastl::is_same<T, bool>::value == false && 
        eastl::is_enum<T>::value == false;
    };

    /**
    * @brief Used to check if a value type is an enumerator
    *
    * @tparam T The type to check
    *
    * @author Daniel Konings
    */
    template <typename T>
    struct is_enum
    {
      /**
      * @brief Is T an enumerator?
      */
      static const bool value =
        is_number<T>::value == false && 
        eastl::is_enum<T>::value == true;
    };

    /**
    * @brief Used to check if a value type is a C-style string
    *
    * @tparam T The type to check
    *
    * @author Daniel Konings
    */
    template <typename T>
    struct is_c_string
    {
      /**
      * @brief Is T a C-style string?
      */
      static const bool value = 
        eastl::is_same<char[
          std::extent<std::remove_reference_t<T>>::value], T>::value;
    };

    /**
    * @brief Used to check if a value type is a vector
    *
    * @remarks Base implementation which is always false
    *
    * @tparam T The type to check
    *
    * @author Daniel Konings
    */
    template <typename T>
    struct is_vector
    {
      /**
      * @brief T is not a vector
      */
      static const bool value = false;
    };

    /**
    * @brief Used to check if a value type is a vector
    *
    * @remarks Specialization for when T is a vector type
    *
    * @tparam T The type to check
    *
    * @author Daniel Konings
    */
    template <typename T>
    struct is_vector<Vector<T>>
    {
      /**
      * @brief T is a vector
      */
      static const bool value = true;
    };

    /**
    * @brief Used to check if T is of a user-defined type
    *
    * @tparam T The type to check
    *
    * @author Daniel Konings
    */
    template <typename T>
    struct is_user_defined
    {
      /**
      * @brief Is T of a user-defined type?
      */
      static const bool value = 
        is_number<T>::value == false && 
        is_enum<T>::value == false && 
        is_c_string<T>::value == false &&
        is_vector<T>::value == false;
    };

    /**
    * @brief Used with SFINAE to compile functions only for number types
    */
    template <typename T>
    using enable_if_number = 
      eastl::enable_if_t<is_number<T>::value == true, T>;

    /**
    * @brief Used with SFINAE to compile functions only for enumerator types
    */
    template <typename T>
    using enable_if_enum = 
      eastl::enable_if_t<is_enum<T>::value == true, T>;

    /**
    * @brief Used with SFINAE to compile functions only for C-style strings
    */
    template <typename T>
    using enable_if_c_string = 
      eastl::enable_if_t<is_c_string<T>::value == true, T>;

    /**
    * @brief Used with SFINAE to compile functions only for user-defined types
    */
    template <typename T>
    using enable_if_user_defined = 
      eastl::enable_if_t<is_user_defined<T>::value == true, T>;

    /**
    * @brief Used with SFINAE to compile functions only for vector types
    */
    template <typename T>
    using enable_if_vector = 
      eastl::enable_if_t<is_vector<T>::value == true, T>;

  }
}
