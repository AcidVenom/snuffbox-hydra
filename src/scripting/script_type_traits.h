#pragma once

#include <EASTL/type_traits.h>

namespace snuffbox
{
  namespace scripting
  {
    /**
    * @brief Used to only enable a function if the template parameter is a
    *        number
    *
    * @tparam T The type to check
    */
    template <typename T>
    using if_number = 
      typename eastl::enable_if<eastl::is_arithmetic<T>::value, T>::type;

    /**
    * @brief Used to only enable a function if the template parameter is an
    *        enumerator
    *
    * @tparam T The type to check
    */
    template <typename T>
    using if_enum = 
      typename eastl::enable_if<eastl::is_enum<T>::value, T>::type;

    /**
    * @brief Used to only enable a function if the template parameter is both
    *        not a number and not an enumerator
    *
    * @tparam T The type to check
    */
    template <typename T>
    using if_n_number_and_enum =
      typename eastl::enable_if<
      eastl::is_arithmetic<T>::value == false &&
      eastl::is_enum<T>::value == false,
      T>::type;
  }
}