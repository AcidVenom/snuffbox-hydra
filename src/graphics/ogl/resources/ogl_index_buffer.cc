#include "graphics/ogl/resources/ogl_index_buffer.h"
#include "graphics/ogl/ogl_utils.h"

namespace snuffbox
{
  namespace graphics
  {
    //--------------------------------------------------------------------------
    OGLIndexBuffer::OGLIndexBuffer() :
      ebo_(0),
      valid_(false),
      size_(0)
    {

    }

    //--------------------------------------------------------------------------
    bool OGLIndexBuffer::Create(const foundation::Vector<Index>& indices)
    {
      Release();

      glGenBuffers(1, &ebo_);

      if (OGLUtils::CheckError() == false)
      {
        return false;
      }

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
      glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, 
        sizeof(Index) * indices.size(), 
        indices.data(), 
        GL_STATIC_DRAW);

      if (OGLUtils::CheckError() == false)
      {
        Release();
        return false;
      }

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

      valid_ = true;
      size_ = indices.size();

      return true;
    }

    //--------------------------------------------------------------------------
    bool OGLIndexBuffer::valid() const
    {
      return valid_;
    }

    //--------------------------------------------------------------------------
    void OGLIndexBuffer::Release()
    {
      if (ebo_ != 0)
      {
        glDeleteBuffers(1, &ebo_);
        ebo_ = 0;
      }

      valid_ = false;
    }

    //--------------------------------------------------------------------------
    OGLIndexBuffer::~OGLIndexBuffer()
    {
      Release();
    }
  }
}