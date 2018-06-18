#include "graphics/ogl/resources/ogl_material.h"
#include "graphics/ogl/resources/ogl_shader.h"

#include <foundation/auxiliary/logger.h>

namespace snuffbox
{
  namespace graphics
  {
    //--------------------------------------------------------------------------
    OGLMaterial::OGLMaterial() :
      program_(0),
      vs_(nullptr),
      ps_(nullptr)
    {

    }

    //--------------------------------------------------------------------------
    bool OGLMaterial::Load(OGLShader* vs, OGLShader* ps)
    {
      Release();

      if (
        vs == nullptr || 
        ps == nullptr || 
        vs->IsValid(ShaderTypes::kVertex) == false || 
        ps->IsValid(ShaderTypes::kPixel) == false)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kEngine,
          foundation::LogSeverity::kError,
          "Attempted to create a material without valid shaders");

        return false;
      }

      vs_ = vs;
      ps_ = ps;

      program_ = glCreateProgram();

      glAttachShader(program_, vs_->shader());
      glAttachShader(program_, ps_->shader());

      glLinkProgram(program_);

      int success;
      char errors[512];

      glGetProgramiv(program_, GL_LINK_STATUS, &success);
      if (success == GL_FALSE) 
      {
        glGetProgramInfoLog(program_, 512, NULL, errors);

        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kEngine,
          foundation::LogSeverity::kError,
          "Could not link shader program for material, errors:\n{0}",
          errors);

        return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------
    bool OGLMaterial::IsValid() const
    {
      return 
        program_ > 0 && 
        vs_ != nullptr && 
        ps_ != nullptr && 
        vs_->IsValid(ShaderTypes::kVertex) && 
        ps_->IsValid(ShaderTypes::kPixel);
    }

    //--------------------------------------------------------------------------
    void OGLMaterial::Set()
    {
      if (IsValid() == false)
      {
        return;
      }

      glUseProgram(program_);
    }

    //--------------------------------------------------------------------------
    void OGLMaterial::Release()
    {
      if (program_ > 0)
      {
        glDeleteProgram(program_);
        program_ = 0;

        vs_ = nullptr;
        ps_ = nullptr;
      }
    }

    //--------------------------------------------------------------------------
    OGLMaterial::~OGLMaterial()
    {
      Release();
    }
  }
}