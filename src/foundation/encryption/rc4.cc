#include "foundation/encryption/rc4.h"

#include <cstring>

#define RC4_SWAP(a, b) ((a) ^= (b), (b) ^= (a), (a) ^= (b))

namespace snuffbox
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    RC4::RC4()
    {
      Clear();
    }

    //--------------------------------------------------------------------------
    void RC4::Encrypt(int8_t* to_encrypt, size_t size, const char* key)
    {
      int i = 0;
      int j = 0;
      int n = 0;

      uint8_t k;

      size_t key_len = strlen(key);

      for (int m = 0; m < kChunkSize_; ++m)
      {
        *(key_ + m) = *(key + (m % key_len));
        *(chunk_ + m) = m;
      }

      for (int m = 0; m < kChunkSize_; ++m)
      {
        n = (n + *(chunk_ + m) + *(key_ + m)) & 0xFF;
        RC4_SWAP(*(chunk_ + m), *(chunk_ + n));
      }

      for (int m = 0; m < size; ++m)
      {
        i = (i + 1) & 0xFF;
        j = (j + *(chunk_ + i)) & 0xFF;
        RC4_SWAP(*(chunk_ + i), *(chunk_ + j));

        k = *(chunk_ + ((*(chunk_ + i) + *(chunk_ + j)) & 0xFF));

        if (k == *(to_encrypt + m))
        {
          k = 0;
        }

        *(to_encrypt + m) ^= k;
      }
    }

    //--------------------------------------------------------------------------
    void RC4::Decrypt(int8_t* to_decrypt, size_t size, const char* key)
    {
      Encrypt(to_decrypt, size, key);
    }

    //--------------------------------------------------------------------------
    void RC4::Clear()
    {
      memset(chunk_, '\0', kChunkSize_);
      memset(key_, '\0', kChunkSize_);
    }

    //--------------------------------------------------------------------------
    RC4::~RC4()
    {
      Clear();
    }
  }
}