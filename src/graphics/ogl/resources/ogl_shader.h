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
      * @brief Loads the shader from SPIRV binary code
      *
      * @param[in] buffer The binary buffer
      * @param[in] len The length of the buffer
      */
      bool Load(const uint8_t* buffer, size_t len);

      /**
      * @return Is the underlying shader loaded correctly?
      *
      * @param[in] requested The requested shader type
      */
      bool IsValid(ShaderTypes requested) const;

      /**
      * @brief Releases the underlying shader
      */
      void Release();

      /**
      * @return The encountered errors
      */
      const foundation::String& error() const;

      /**
      * @return The underlying shader
      */
      GLuint shader() const;

      /**
      * @retunr The type of this shader
      */
      ShaderTypes type() const;

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