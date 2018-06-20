#pragma once

#include <glad/glad.h>

#include <cstddef>

namespace snuffbox
{
  namespace graphics
  {
    /**
    * @brief Used to wrap around OpenGL's uniform buffer objects
    *
    * This is used in conjuction with the PerFrameData and PerObjectData
    * to set shader constants without having to update the data per program
    *
    * @author Daniel Konings
    */
    class OGLUniformBuffer
    {

    public:

      /**
      * @brief Creates an invalid buffer, Update should be called on it
      *        to create it
      */
      OGLUniformBuffer();

      /**
      * @brief Is this buffer valid?
      */
      bool IsValid() const;

      /**
      * @brief Updates the contents of this buffer
      *
      * @remarks This will create the buffer if the current buffer is invalid
      *          or if the size is different
      *
      * @param[in] buffer The buffer to set
      * @param[in] len The length of the buffer
      */
      void Update(const void* buffer, size_t len);

      /**
      * @brief Binds the buffer to a program at a specified location
      *
      * @param[in] location The location to bind the buffer at
      */
      void Set(GLuint location);

    protected:

      /**
      * @brief Releases the created buffer if it was created
      */
      void Release();

    public:

      /**
      * @see OGLUniformBuffer::Release
      */
      ~OGLUniformBuffer();

    private:

      GLuint ubo_; //!< The underlying uniform buffer object
      size_t size_; //!< The current size of the buffer
    };
  }
}