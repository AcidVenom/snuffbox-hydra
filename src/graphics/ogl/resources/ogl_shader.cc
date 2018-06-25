#include "graphics/ogl/resources/ogl_shader.h"
#include "graphics/ogl/ogl_utils.h"

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
    bool OGLShader::Load(const uint8_t* buffer, size_t len)
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

      glShaderBinary(
        1, 
        &shader_, 
        GL_SHADER_BINARY_FORMAT_SPIR_V, 
        buffer, 
        static_cast<GLsizei>(len));

      if (OGLUtils::CheckError() == false)
      {
        error_ = "SPIRV unsupported in OpenGL versions lower than 4.6";
        Release();
        return false;
      }

      glSpecializeShader(shader_, "main", 0, nullptr, nullptr);

      if (OGLUtils::CheckError() == false)
      {
        Release();
        return false;
      }

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