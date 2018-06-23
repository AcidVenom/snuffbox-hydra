#pragma once

#include <glad/glad.h>

namespace snuffbox
{
  namespace graphics
  {
    class OGLShader;

    /**
    * @brief A class to link multiple shaders together into a single
    *        shader program
    *
    * @author Daniel Konings
    */
    class OGLMaterial
    {

    public:

      /**
      * @brief Default constructor
      */
      OGLMaterial();

      /**
      * @brief Loads a material from its respective shaders
      *
      * @param[in] vs The vertex shader
      * @param[in] ps The pixel shader
      * @param[in] gs The geometry shader, or nullptr if unused
      *
      * @return Were we able to link the shader program correctly?
      */
      bool Load(OGLShader* vs, OGLShader* ps, OGLShader* gs);

      /**
      * @return Is this material valid?
      *
      * @remarks A material is only valid when its shader program is linked and
      *          the respective shaders are still loaded
      */
      bool IsValid() const;

      /**
      * @brief Sets this material as the current material used for rendering
      */
      void Set();

      /**
      * @brief Releases the underlying program
      */
      void Release();

      /**
      * @see OGLMaterial::Release
      */
      ~OGLMaterial();

    private:

      GLuint program_; //!< The shader program of this material

      OGLShader* vs_; //!< The vertex shader of the 
      OGLShader* ps_; //!< The pixel shader of the material
      OGLShader* gs_; //!< The geometry shader of the material
    };
  }
}