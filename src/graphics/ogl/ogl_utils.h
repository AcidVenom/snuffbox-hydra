#pragma once

#include <glad/glad.h>

namespace snuffbox
{
  namespace graphics
  {
    /**
    * @brief Used to provide helper functionality while writing the OpenGL
    *        API implementation
    *
    * @author Daniel Konings
    */
    class OGLUtils
    {

    public:

      /**
      * @brief Checks for errors using glGetError and logs human-readable
      *        strings whenever anything other than GL_NO_ERROR is found
      *
      * @return Are there no errors?
      */
      static bool CheckError();
    };
  }
}