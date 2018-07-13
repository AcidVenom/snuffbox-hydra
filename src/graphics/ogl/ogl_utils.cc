#include "graphics/ogl/ogl_utils.h"

#include <foundation/auxiliary/logger.h>

namespace snuffbox
{
  namespace graphics
  {
    //--------------------------------------------------------------------------
    bool OGLUtils::CheckError()
    {
      GLenum err = glGetError();
      foundation::String msg = "";

      bool no_error = err == GL_NO_ERROR;

      while(err != GL_NO_ERROR)
      {
        switch(err) {
        case GL_INVALID_OPERATION:
          msg = "INVALID_OPERATION";
          break;
        case GL_INVALID_ENUM:
          msg = "INVALID_ENUM";
          break;
        case GL_INVALID_VALUE:
          msg = "INVALID_VALUE";
          break;
        case GL_OUT_OF_MEMORY:
          msg = "OUT_OF_MEMORY";
          break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
          msg = "INVALID_FRAMEBUFFER_OPERATION";
          break;
        }

        foundation::Logger::LogVerbosity<2>(
          foundation::LogChannel::kEngine,
          foundation::LogSeverity::kError,
          "OpenGL error: GL_{0}",
          msg);

        err = glGetError();
      }

      return no_error;
    }
  }
}