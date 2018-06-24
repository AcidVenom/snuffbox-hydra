#include "graphics/ogl/resources/ogl_uniform_buffer.h"
#include "graphics/ogl/ogl_utils.h"

namespace snuffbox
{
  namespace graphics
  {
    //--------------------------------------------------------------------------
    OGLUniformBuffer::OGLUniformBuffer() :
      ubo_(0),
      size_(0)
    {

    }

    //--------------------------------------------------------------------------
    bool OGLUniformBuffer::IsValid() const
    {
      return ubo_ != 0 && size_ > 0;
    }

    //--------------------------------------------------------------------------
    void OGLUniformBuffer::Update(const void* buffer, size_t len)
    {
      if (buffer == nullptr || len == 0)
      {
        return;
      }

      if (ubo_ == 0 || size_ != len)
      {
        Release();
        glGenBuffers(1, &ubo_);

        glBindBuffer(GL_UNIFORM_BUFFER, ubo_);

        glBufferData(
          GL_UNIFORM_BUFFER, 
          static_cast<GLsizeiptr>(len), 
          buffer, 
          GL_DYNAMIC_DRAW);

        if (OGLUtils::CheckError() == false)
        {
          return;
        }
      }
      else
      {
        glBindBuffer(GL_UNIFORM_BUFFER, ubo_);
      }

      size_ = len;

      GLvoid* p = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
      memcpy(p, buffer, len);
      glUnmapBuffer(GL_UNIFORM_BUFFER);

      glBindBuffer(GL_UNIFORM_BUFFER, 0);

      OGLUtils::CheckError();
    }

    //--------------------------------------------------------------------------
    void OGLUniformBuffer::Set(GLuint location)
    {
      if (IsValid() == false)
      {
        return;
      }

      glBindBuffer(GL_UNIFORM_BUFFER, ubo_);
      glBindBufferBase(GL_UNIFORM_BUFFER, location, ubo_);
    }

    //--------------------------------------------------------------------------
    void OGLUniformBuffer::Release()
    {
      if (ubo_ > 0)
      {
        glDeleteBuffers(1, &ubo_);
        ubo_ = 0;
        size_ = 0;
      }
    }

    //--------------------------------------------------------------------------
    OGLUniformBuffer::~OGLUniformBuffer()
    {
      Release();
    }
  }
}