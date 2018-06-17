#pragma once

#include "graphics/definitions/shader_types.h"

#include <foundation/containers/string.h>

#include <glad/glad.h>

namespace snuffbox
{
  namespace graphics
  {
    /**
    * @brief Used to wrap separate OpenGL shaders, this includes vertex, pixel
    *        and geometry shaders
    *
    * @author Daniel Konings
    */
    class OGLShader
    {

    public:

      /**
      * @brief Construct the shader through a type
      *
      * @param[in] type The type of this shader
      */
      OGLShader(ShaderTypes type);

      /**
      * @brief Loads the shader from GLSL source code
      *
      * @param[in] src The source code
      */
      bool Load(const foundation::String& src);

      /**
      * @return The encountered errors
      */
      const foundation::String& error() const;

      /**
      * @return The underlying shader
      */
      GLuint shader() const;

    protected:

      /**
      * @brief Releases the underlying shader
      */
      void Release();

    public:

      /**
      * @brief Releases the shader
      *
      * @see OGLShader::Release
      */
      ~OGLShader();

    private:

      ShaderTypes type_; //!< The type of this shader
      foundation::String error_; //!< The errors encountered

      GLuint shader_; //!< The underlying shader
    };
  }
}