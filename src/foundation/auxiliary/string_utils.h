#pragma once

#include "foundation/containers/string.h"
#include "foundation/containers/vector.h"
#include "foundation/io/path.h"
#include "foundation/containers/uuid.h"

#include <glm/glm.hpp>

#include <sstream>
#include <cinttypes>

namespace snuffbox
{
  namespace foundation
  {
    /**
    * @brief Useful helper functions for regular string operations
    *
    * @author Daniel Konings
    */
    class StringUtils
    {

    public:

      /**
      * @brief A type definition for a list of strings
      */
      using StringList = Vector<String>;

      /**
      * @brief Splits a string by a single delimiter
      *
      * @param[in] str The string to split
      * @param[in] delim The delimiter
      *
      * @return The split string as a vector of strings
      */
      static StringList Split(const String& str, char delim);

      /**
      * @brief Checks if a token is contained within a string
      *
      * @param[in] str The string to check for the token
      * @param[in] token The token
      *
      * @return The index at which the token was first found, 
      *         or -1 if not found
      */
      static int Contains(const String& str, const char* token);

      /**
      * @brief Finds the last index of a character or String::npos if it
      *        is not in the string
      *
      * @param[in] str The string to check
      * @param[in] token The token to check for
      *
      * @return The found position
      */
      static size_t FindLastOf(const String& str, char token);

      /**
      * @brief Returns the number of occurences of a token in the string
      *
      * @param[in] str The string to check
      * @param[in] token The token to check for
      *
      * @return The number of occurences of the token in the string
      */
      static size_t NumberOf(const String& str, char token);

      /**
      * @brief Converts a value to a string
      *
      * @tparam T The value type to convert
      *
      * @param[in] value The value to convert
      *
      * @return The converted value
      */
      template <typename T>
      static String ToString(const T& value);
    };

    //--------------------------------------------------------------------------
    template <typename T>
    inline String StringUtils::ToString(const T& value)
    {
      std::ostringstream stream;
      stream << value;

      return stream.str().c_str();
    }

    //--------------------------------------------------------------------------
    template <>
    inline String StringUtils::ToString(const bool& value)
    {
      return value == true ? "true" : "false";
    }

    //--------------------------------------------------------------------------
    template <>
    inline String StringUtils::ToString(const String& value)
    {
      return value;
    }

    //--------------------------------------------------------------------------
    template <>
    inline String StringUtils::ToString(const Path& value)
    {
      return value.ToString();
    }

    //--------------------------------------------------------------------------
    template <>
    inline String StringUtils::ToString(const UUID& value)
    {
      return value.ToString();
    }

    //--------------------------------------------------------------------------
    template <>
    inline String StringUtils::ToString(const glm::vec2& value)
    {
      return 
        "[" + 
        ToString<float>(value.x) + ", " + 
        ToString<float>(value.y) + 
        "]";
    }

    //--------------------------------------------------------------------------
    template <>
    inline String StringUtils::ToString(const glm::vec3& value)
    {
      return
        "[" +
        ToString<float>(value.x) + ", " +
        ToString<float>(value.y) + ", " +
        ToString<float>(value.z) +
        "]";
    }

    //--------------------------------------------------------------------------
    template <>
    inline String StringUtils::ToString(const glm::vec4& value)
    {
      return
        "[" +
        ToString<float>(value.x) + ", " +
        ToString<float>(value.y) + ", " +
        ToString<float>(value.z) + ", " +
        ToString<float>(value.w) +
        "]";
    }
  }
}