#pragma once

#include "graphics/ogl/resources/ogl_vertex_buffer.h"
#include "graphics/ogl/resources/ogl_index_buffer.h"

namespace snuffbox
{
  namespace graphics
  {
    /**
    * @brief Used to contain both a vertex and an index buffer
    *
    * @remarks This class should be held as GPU handle by an asset
    *
    * @author Daniel Konings
    */
    class OGLMesh
    {

    public:

      /**
      * @brief Default constructor, creates an invalid mesh
      */
      OGLMesh();

      /**
      * @brief Creates both a vertex buffer and index buffer for this
      *        mesh
      *
      * @param[in] vertices The vertices to create the vertex buffer with
      * @param[in] indices The indices to create the index buffer with
      *
      * @return Were both the vertex and index buffer created succesfully?
      */
      template <typename T>
      bool Create(
        const foundation::Vector<T>& vertices, 
        const foundation::Vector<Index>& indices);

      /**
      * @return Is both the vertex buffer and index buffer valid?
      */
      bool IsValid() const;

    private:

      OGLVertexBuffer vertex_buffer_; //!< The vertex buffer
      OGLIndexBuffer index_buffer_; //!< The index buffer
    };

    //--------------------------------------------------------------------------
    template <typename T>
    inline bool OGLMesh::Create(
      const foundation::Vector<T>& vertices, 
      const foundation::Vector<Index>& indices)
    {
      if (vertex_buffer_.Create<T>(vertices) == false)
      {
        return false;
      }
      
      return index_buffer_.Create(indices);
    }
  }
}