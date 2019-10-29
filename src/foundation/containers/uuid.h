#pragma once

#include <foundation/containers/string.h>

#include <cinttypes>

#define UUID_SEGMENTS 4

namespace snuffbox
{
  namespace foundation
  {
    /**
    * @brief A naive implementation for UUIDs, so that we can have
    *        unique identifiers through our code-base
    *
    * @author Daniel Konings
    */
    class UUID
    {

      using data_type = int32_t;

    public:

      /**
      * @brief Generates an empty UUID
      */
      UUID();

      /**
      * @brief Generates a new UUID pseudo-randomly
      *
      * @return The new UUID
      */
      static UUID Create();

      /**
      * @brief Instantiate a UUID from a string value
      *
      * @return The UUID, or a null UUID if the string value was not valid
      */
      static UUID FromString(const String& str);

      /**
      * @return Is this UUID equal to another UUID?
      */
      bool operator==(const UUID& other) const;

      /**
      * @return Is this UUID not equal to another UUID?
      */
      bool operator!=(const UUID& other) const;

      /**
      * @return Is this UUID null?
      */
      bool IsNull() const;

      /**
      * @return The UUID as a hex string contained in brackets, e.g.
      *         {0000-0000-0000-0000}
      */
      String ToString() const;

    private:

      /**
      * @brief 4 sections of data, represented by the data type
      */
      data_type data_[UUID_SEGMENTS];
    };
  }
}