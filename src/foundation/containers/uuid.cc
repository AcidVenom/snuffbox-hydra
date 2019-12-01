#include "foundation/containers/uuid.h"

#include <cstdlib>
#include <ctime>
#include <limits>
#include <cstring>
#include <random>

#if defined (SNUFF_WIN32)
#define sprintf(buffer, size, format, ...)\
sprintf_s(buffer, size, format, __VA_ARGS__)
#elif defined (SNUFF_LINUX)
#define sprintf(buffer, size, format, ...)\
sprintf(buffer, format, __VA_ARGS__)
#else
#error Unknown platform
#endif

namespace snuffbox
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    UUID::UUID()
    {
      memset(data_, 0, sizeof(data_));
    }

    //--------------------------------------------------------------------------
    UUID UUID::Create()
    {
      UUID uuid;

      static std::uniform_int_distribution<data_type> distribution;
      static std::random_device random_device;
      static std::mt19937_64 twister;

      static bool randomized = false;
      if (randomized == false)
      {
        twister = std::mt19937_64(random_device());
        randomized = true;
      }

      data_type segment = data_type(0);

      for (size_t i = 0; i < UUID_SEGMENTS; ++i)
      {
        segment = distribution(twister);
        uuid.data_[i] = segment;
      }

      return uuid;
    }

    //--------------------------------------------------------------------------
    UUID UUID::FromString(const String& str)
    {
      UUID uuid;
      const int hex_length = sizeof(data_type) * 2;
      const int brackets_length = 2;
      const int dash_length = UUID_SEGMENTS - 1;

      if (str.size() != 
        hex_length * UUID_SEGMENTS + brackets_length + dash_length)
      {
        return uuid;
      }

      char buffer[hex_length + 1 + 2];
      buffer[0] = '0';
      buffer[1] = 'x';
      buffer[2 + hex_length] = '\0';

      for (size_t i = 0; i < UUID_SEGMENTS; ++i)
      {
        const char* current_segment = &str.at((i * hex_length) + 1 + i);
        memcpy(buffer + 2, current_segment, hex_length);

        unsigned long long ull = strtoull(current_segment, nullptr, 16);
        uuid.data_[i] = static_cast<data_type>(ull);
      }

      return uuid;
    }

    //--------------------------------------------------------------------------
    bool UUID::operator==(const UUID& other) const
    {
      return memcmp(data_, other.data_, sizeof(data_)) == 0;
    }

    //--------------------------------------------------------------------------
    bool UUID::operator!=(const UUID& other) const
    {
      return (*this == other) == false;
    }

    //--------------------------------------------------------------------------
    bool UUID::IsNull() const
    {
      UUID null;
      return *this == null;
    }

    //--------------------------------------------------------------------------
    String UUID::ToString() const
    {
      const int num_brackets = 2;
      const int num_dashes = UUID_SEGMENTS - 1;
      const int hex_length = sizeof(data_type) * 2;
      const int str_len = 
        num_brackets + num_dashes + hex_length * UUID_SEGMENTS;

      char buffer[str_len + 1];
      buffer[0] = '{';
      buffer[str_len - 1] = '}';
      buffer[str_len] = '\0';

      int offset = 1;

      char format_buffer[hex_length + 1];
      memset(format_buffer, '0', hex_length);
      memset(format_buffer + hex_length, '\0', 1);

      for (size_t i = 0; i < UUID_SEGMENTS; ++i)
      {
        data_type segment = data_[i];

        sprintf(
          format_buffer, 
          sizeof(format_buffer), 
          "%X", 
          segment);

        size_t len = strlen(format_buffer);

        size_t shift = hex_length - len;

        if (shift > 0)
        {
          memset(format_buffer, '0', shift);
        }

        sprintf(
          format_buffer + shift, 
          sizeof(format_buffer) - shift, 
          "%X", 
          segment);

        memcpy(buffer + offset, format_buffer, hex_length);

        if (i == UUID_SEGMENTS - 1)
        {
          break;
        }

        offset += hex_length;
        buffer[offset] = '-';

        ++offset;
      }

      return buffer;
    }
  }
}