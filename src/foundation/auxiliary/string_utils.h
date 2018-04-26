#pragma once

#include <foundation/containers/string.h>
#include <foundation/containers/vector.h>

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
    };
  }
}