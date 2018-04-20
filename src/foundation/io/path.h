#pragma once

#include <foundation/containers/string.h>

namespace snuffbox
{
  namespace foundation
  {
    /**
    * @brief A string container that acts as a file system path
    *
    * The path is a cross-platform solution to the issue with backslashes on
    * Windows. On top of that, it provides some functionality for accessing
    * the virtual resource file system and operators to add to the path.
    *
    * If there is a slash at the end of the path, it is implicitly removed
    *
    * @author Daniel Konings
    */
    class Path
    {

    public:

      /**
      * @brief Default constructor, construct an empty path
      */
      Path();

      /**
      * @brief Constructs a path from an initial value, any backslashes
      *        will be converted to forward slashes
      *
      * @remarks This also removes the trailing slash at the end of the path,
      *          if any
      */
      Path(const String& path);

      /**
      * @see Path::Path
      *
      * @remarks const char* overload
      */
      Path(const char* path);

      /**
      * @brief Appends a string to the path while also inserting a new
      *        slash if there is not one already
      */
      void operator/=(const String& other);

      /**
      * @see Path::operator/=
      *
      * @remarks Path overload
      */
      void operator/=(const Path& other);

      /**
      * @see Path::operator/=
      */
      Path operator/(const String& other);

      /**
      * @see Path::operator/=
      */
      Path operator/(const Path& other);

      /**
      * @brief Appends a string to the path, but doesn't insert a new
      *        slash, slashes are still converted
      */
      void operator+=(const String& other);

      /**
      * @see Path::operator+=
      *
      * @remarks Path overload
      */
      void operator+=(const Path& other);

      /**
      * @see Path::operator+=
      */
      Path operator+(const String& other);

      /**
      * @see Path::operator+=
      */
      Path operator+(const Path& other);

      /**
      * @brief Assigns a path from a string
      *
      * @see Path::Path
      *
      * @return The new path
      */
      Path operator=(const String& other);

      /**
      * @brief Checks if a path and string are equal
      *
      * @param[in] other The string to check against
      *
      * @return Are they equal?
      */
      bool operator==(const String& other);

      /**
      * @see Path::operator==
      *
      * @remarks Path overload
      */
      bool operator==(const Path& other);

      /**
      * @see Path::operator==
      *
      * @remarks Checks if not equal, by negating the equals operator
      */
      bool operator!=(const String& other);

      /**
      * @see Path::operator==
      *
      * @remarks Checks if not equal, by negating the equals operator
      */
      bool operator!=(const Path& other);

      /**
      * @return The underlying string value of the path
      */
      const String& ToString() const;

      /**
      * @return The path without any virtual prefixes
      */
      Path StripVirtualPrefix() const;

      /**
      * @return Is this path a virtual path into the virtual file system?
      */
      bool is_virtual() const;

    protected:

      /**
      * @brief Prepends a slash to a string, if there is not one there already
      *
      * @remarks This also calls Path::ConvertSlashes on the string
      *
      * @param[in] str The string to prepend the slash to
      *
      * @return The new string
      */
      static String PrependSlash(const String& str);

      /**
      * @brief Converts any backslashes in a string to forward slashes and
      *        removes the starting/trailing slash if it exists
      *
      * @remarks This function also makes sure that any duplicate slashes
      *          are removed accordingly
      *
      * @param[in] str The string to convert the slashes of
      *
      * @return The converted string
      */
      static String ConvertSlashes(const String& str);

      /**
      * @brief Checks if a path is virtual by looking for the prefix defined in
      *        Path::kVirtualPrefix_
      *
      * @param[in] str The stringified path
      */
      static bool IsVirtualPath(const String& str);

    private:

      static const char* kVirtualPrefix_; //!< The virtual path prefix

      String path_; //!< The stringified path
      bool is_virtual_; //!< Is this a virtual path to the virtual file system?

    };
  }
}