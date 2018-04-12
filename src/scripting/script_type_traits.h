#pragma once

#include <EASTL/type_traits.h>

namespace snuffbox
{
  namespace scripting
  {
    /**
    * @brief Used to only enable a function if the template parameter is a
    *        number or enumerator, but not a boolean
    *
    * @tparam T The type to check
    */
    template <typename T>
    using if_number_or_enum = 
      typename eastl::enable_if<
      (eastl::is_arithmetic<T>::value == true || 
      eastl::is_enum<T>::value == true) &&
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
  }
}