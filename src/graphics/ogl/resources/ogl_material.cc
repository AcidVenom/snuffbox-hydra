#include "graphics/ogl/resources/ogl_material.h"
#include "graphics/ogl/resources/ogl_shader.h"
#include "graphics/ogl/ogl_utils.h"
#include "graphics/definitions/shader_constants.h"

#include <foundation/auxiliary/logger.h>

namespace snuffbox
{
  namespace graphics
  {
    //--------------------------------------------------------------------------
    OGLMaterial::OGLMaterial() :
      program_(0),
      vs_(nullptr),
      ps_(nullptr),
      gs_(nullptr)
    {

    }

    //--------------------------------------------------------------------------
    bool OGLMaterial::Load(OGLShader* vs, OGLShader* ps, OGLShader* gs)
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

      if (gs != nullptr && gs->IsValid(ShaderTypes::kGeometry) == false)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kEngine,
          foundation::LogSeverity::kError,
          "Attempted to create a material with an invalid geometry shader");

        return false;
      }

      vs_ = vs;
      ps_ = ps;
      gs_ = gs;

      program_ = glCreateProgram();

      glAttachShader(program_, vs_->shader());
      glAttachShader(program_, ps_->shader());

      if (gs_ != nullptr)
      {
        glAttachShader(program_, gs_->shader());
      }

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

      glUniformBlockBinding(
        program_, 
        ShaderConstants::GetUniformLocation(ShaderUniforms::kPerFrameData),
        0);

      if (OGLUtils::CheckError() == false)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kEngine,
          foundation::LogSeverity::kWarning,
          "Could not find per-frame-data uniform location in material");
      }

      glUniformBlockBinding(
        program_, 
        ShaderConstants::GetUniformLocation(ShaderUniforms::kPerObjectData),
        1);

      if (OGLUtils::CheckError() == false)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kEngine,
          foundation::LogSeverity::kWarning,
          "Could not find per-object-data uniform location in material");
      }

      return true;
    }

    //--------------------------------------------------------------------------
    bool OGLMaterial::IsValid() const
    {
      bool check_a =  
        program_ > 0 && 
        vs_ != nullptr && 
        ps_ != nullptr && 
        vs_->IsValid(ShaderTypes::kVertex) == true && 
        ps_->IsValid(ShaderTypes::kPixel) == true;

      bool check_b = 
        gs_ == nullptr || 
        gs_->IsValid(ShaderTypes::kGeometry) == true;

      return check_a == true && check_b == true;
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
        gs_ = nullptr;
      }
    }

    //--------------------------------------------------------------------------
    OGLMaterial::~OGLMaterial()
    {
      Release();
    }
  }
}