#include "graphics/ogl/resources/ogl_vertex_buffer.h"

namespace snuffbox
{
  namespace graphics
  {
    //--------------------------------------------------------------------------
    OGLVertexBuffer::OGLVertexBuffer() :
      vbo_(0),
      vao_(0),
      valid_(false),
      size_(0)
    {

    }

    //--------------------------------------------------------------------------
    bool OGLVertexBuffer::valid() const
    {
      return valid_;
    }

    //--------------------------------------------------------------------------
    size_t OGLVertexBuffer::size() const
    {
      return size_;
    }

    //--------------------------------------------------------------------------
    void OGLVertexBuffer::Release()
    {
      if (vbo_ != 0)
      {
        glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
      }

      if (vao_ != 0)
      {
        glDeleteBuffers(1, &vao_);
        vao_ = 0;
      }

      valid_ = false;
    }

    //--------------------------------------------------------------------------
    OGLVertexBuffer::~OGLVertexBuffer()
    {
      Release();
    }
  }
}