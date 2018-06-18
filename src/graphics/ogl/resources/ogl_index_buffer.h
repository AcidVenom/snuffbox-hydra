#pragma once

#include "graphics/definitions/vertex.h"

#include <foundation/containers/vector.h>

#include <glad/glad.h>

#include <cinttypes>
#include <cstddef>

namespace snuffbox
{
  namespace graphics
  {
    /**
    * @brief Used to create index buffers in OpenGL, from a list of indices
    */
    class OGLIndexBuffer
    {

    public:

      /**
      * @brief Default constructor, creates an invalid index buffer
      */
      OGLIndexBuffer();

      /**
      * @brief Creates the buffer from a list of indices
      *
      * @param[in] indices The list of indices
      *
      * @return Were we able to create the index buffer?
      */
      bool Create(const foundation::Vector<Index>& indices);

      /**
      * @brief Sets this index buffer as the active index buffer
      */
      void Set();

      /**
      * @return Is this index buffer valid for use?
      */
      bool valid() const;

      /**
      * @return The size of the indices
      */
      size_t size() const;

    protected:

      /**
      * @brief Frees up the GPU memory if allocated
      *
      * @remarks This method is safe to call whether this index buffer is valid
      *          or not, as this is checked internally
      */
      void Release();

    public:

      /**
      * @brief Default destructor
      *
      * @see OGLIndexBuffer::Release
      */
      ~OGLIndexBuffer();

    private:

      GLuint ebo_; //!< The element buffer object
      bool valid_; //!< Is this index buffer valid for use?
      size_t size_; //!< The size of the indices
    };
  }
}