#pragma once

#include <cinttypes>
#include <cstddef>

namespace snuffbox
{
  namespace foundation
  {
    /**
    * @brief Used as a short-hand for pointer arithmetic
    *
    * Avoids the need for casting pointers back and forth and provides
    * some utility functionality as well (e.g. alignment).
    *
    * @author Daniel Konings
    */
    class PointerMath
    {

    public:

      /**
      * @brief Aligns a pointer to the next alignment boundary and returns the
      *        offset required to align it
      *
      * @param[in] ptr The pointer to align
      * @param[in] align The alignment
      *
      * @return The offset required to reach the alignment boundary
      */
      static size_t AlignDelta(void* ptr, size_t align);

      /**
      * @brief Offset the pointer by an integer value
      *
      * @param[in] ptr The pointer to offset
      * @param[in] offset The amount to offset by, in bytes
      *
      * @return The offsetted pointer
      */
      static void* Offset(void* ptr, int64_t offset);

      /**
      * @see PointerMath::Offset
      *
      * @remarks const version
      */
      static const void* Offset(const void* ptr, int64_t offset);
    };
  }
}
