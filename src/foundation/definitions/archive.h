#pragma once

#include "foundation/serialization/serializable.h"
#include "foundation/auxiliary/type_traits.h"

#define ARCHIVE_PROP(x) snuffbox::foundation::ArchiveName{ #x }, ## x

namespace snuffbox
{
  namespace foundation
  {
    /**
    * @brief Used as a unique type to store property names
    *
    * @author Daniel Konings
    */
    struct ArchiveName
    {
      const char* name; //!< The name of the property
    };

    /**
    * @brief Used to check if a type derives from ISerializable
    *
    * @tparam T The type to check
    *
    * @author Daniel Konings
    */
    template <typename T>
    struct is_serializable
    {
      /**
      * @brief Is T serializable?
      */
      static const bool value = 
        eastl::is_base_of<ISerializable, 
        typename eastl::remove_pointer<T>::type>::value;
    };

    /**
    * @brief Used to enable functions when T is of a user-defined type and
    *        serializable
    *
    * @tparam T The type to check
    *
    * @author Daniel Konings
    */
    template <typename T>
    using enable_if_serializable = 
      eastl::enable_if_t<is_serializable<T>::value == true, T>;

    /**
    * @brief Used to enable functions when T is of a user-defined type and
    *        not serializable
    *
    * @tparam T The type to check
    *
    * @author Daniel Konings
    */
    template <typename T>
    using enable_if_n_serializable =
      eastl::enable_if_t<is_user_defined<T>::value == true && 
      is_serializable<T>::value == false, T>;
  }
}