#pragma once

#include <cinttypes>
#include <cstddef>

namespace snuffbox
{
  namespace foundation
  {
    /**
    * @brief RC4 encryption algorithm, to convert string data into an unreadable
    *        format
    *
    * This is a pretty naive technique to "encrypt" files, as Snuffbox
    * is open-source. However, I personally don't want to make it too easy
    * to edit the compiled script files that are used in a build.
    *
    * Anything can be decompiled technically, so this is an easy lightweight way
    * to simply make it a tad more difficult.
    *
    * By default, the RC4 key is salted with SNUFF_ENCRYPTION_KEY from CMake.
    *
    * @author Daniel Konings
    */
    class RC4
    {

    public:

      /**
      * @brief Default constructor, starts a new chunk
      */
      RC4();

      /**
      * @brief Encrypts a character array with RC4
      *
      * The array is directly modified, so the encryption algorithm doens't
      * allocate any data.
      *
      * The size of the resulting array will remain the same size as the
      * original array.
      *
      * @param[in] to_encrypt The character array to encrypt
      * @param[in] size The size of the character array
      * @param[in] key The encryption key to use
      */
      void Encrypt(
        int8_t* to_encrypt, 
        size_t size, 
        const char* key = SNUFF_ENCRYPTION_KEY);

      /**
      * @brief Decrypts a previously encrypted RC4 string
      *
      * This basically runs RC4::Encrypt again, as a trait of RC4
      * is that running the encrypt algorithm again results in the
      * decrypted string (as long as the key remains the same).
      *
      * @see RC4::Encrypt
      */
      void Decrypt(
        int8_t* to_decrypt,
        size_t size,
        const char* key = SNUFF_ENCRYPTION_KEY);

    protected:

      /**
      * @brief Clears the current encryption chunk and key
      */
      void Clear();

    public:

      /**
      * @brief Guarantees the RC4 encrpytion is cleared during destruction
      *        to not have it linger in memory
      */
      ~RC4();

    private:

      static const size_t kChunkSize_ = 256; //!< The chunk size for encryption

      uint8_t chunk_[kChunkSize_]; //!< The encryption chunk
      uint8_t key_[kChunkSize_]; //!< The encryption key
    };
  }
}