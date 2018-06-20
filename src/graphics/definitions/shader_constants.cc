#include "graphics/definitions/shader_constants.h"

namespace snuffbox
{
  namespace graphics
  {
    //--------------------------------------------------------------------------
    uint32_t ShaderConstants::GetUniformLocation(ShaderUniforms uniform)
    {
      return static_cast<uint32_t>(uniform);
    }
  }
}