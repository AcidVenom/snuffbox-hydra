#include "graphics/ogl/resources/ogl_shader.h"

namespace snuffbox
{
  namespace graphics
  {
    //--------------------------------------------------------------------------
    OGLShader::OGLShader(ShaderTypes type) :
      type_(type),
      error_(""),
      shader_(0)
    {

    }

    //--------------------------------------------------------------------------
    bool OGLShader::Load(const foundation::String& src)
    {
      Release();

      error_ = "No errors";

      GLuint type = 0;

      switch (type_)
      {
      case ShaderTypes::kVertex:
        type = GL_VERTEX_SHADER;
        break;

      case ShaderTypes::kPixel:
        type = GL_FRAGMENT_SHADER;
        break;

      case ShaderTypes::kGeometry:
        type = GL_GEOMETRY_SHADER;
        break;
      }

      if (type == 0)
      {
        error_ = "Unsupported shader type";
        return false;
      }

      shader_ = glCreateShader(type);

      const char* c_src = src.c_str();

      glShaderSource(shader_, 1, &c_src, NULL);
      glCompileShader(shader_);

      int success;
      char errors[512];
      glGetShaderiv(shader_, GL_COMPILE_STATUS, &success);

      if (success == GL_FALSE)
      {
        glGetShaderInfoLog(shader_, 512, NULL, errors);
        error_ = errors;

        return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------
    bool OGLShader::IsValid(ShaderTypes requested) const
    {
      return type_ == requested && shader_ > 0;
    }

    //--------------------------------------------------------------------------
    void OGLShader::Release()
    {
      if (shader_ > 0)
      {
        glDeleteShader(shader_);
        shader_ = 0;
      }
    }

    //--------------------------------------------------------------------------
    const foundation::String& OGLShader::error() const
    {
      return error_;
    }

    //--------------------------------------------------------------------------
    GLuint OGLShader::shader() const
    {
      return shader_;
    }

    //--------------------------------------------------------------------------
    ShaderTypes OGLShader::type() const
    {
      return type_;
    }

    //--------------------------------------------------------------------------
    OGLShader::~OGLShader()
    {
      Release();
    }
  }
}