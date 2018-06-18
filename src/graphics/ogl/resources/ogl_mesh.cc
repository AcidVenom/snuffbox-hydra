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

    //--------------------------------------------------------------------------
    void OGLMesh::Set()
    {
      if (IsValid() == false)
      {
        return;
      }

      vertex_buffer_.Set();
      index_buffer_.Set();
    }

    //--------------------------------------------------------------------------
    size_t OGLMesh::NumIndices() const
    {
      if (IsValid() == false)
      {
        return 0;
      }

      return index_buffer_.size();
    }
  }
}