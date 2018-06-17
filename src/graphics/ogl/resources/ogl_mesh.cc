#include "graphics/ogl/resources/ogl_mesh.h"

namespace snuffbox
{
  namespace graphics
  {
    //--------------------------------------------------------------------------
    OGLMesh::OGLMesh()
    {

    }

    //--------------------------------------------------------------------------
    bool OGLMesh::IsValid() const
    {
      return vertex_buffer_.valid() == true && index_buffer_.valid() == true;
    }
  }
}