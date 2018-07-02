#pragma once

#include "graphics/definitions/vertex.h"
#include "graphics/ogl/ogl_utils.h"

#include <foundation/containers/vector.h>
#include <foundation/auxiliary/logger.h>

#include <glad/glad.h>

#include <cstddef>

namespace snuffbox
{
  namespace graphics
  {
    class OGLMesh;

    /**
    * @brief Used to create vertex buffers in OpenGL 
    *
    * @author Daniel Konings
    */
    class OGLVertexBuffer
    {

      friend OGLMesh;

    public:

      /**
      * @brief Default constructor, creates an invalid vertex buffer
      */
      OGLVertexBuffer();

    protected:

      /**
      * @brief A helper function to automatically offset the vertex attribute
      *        pointer based on an index and number of elements for floats
      *
      * @param[in] i The attribute number
      * @param[in] n The number of floats to set in the attribute
      * @param[in] prev The previous number of float elements
      *
      * @return The current number of float elements
      */
      template <typename T>
      static GLuint SetAttributeFloat(GLuint i, GLint n, GLint prev);

      /**
      * @brief Sets the attributes of a vertex array object
      *
      * This function expects the generated, empty, vertex array object to
      * be bound using glBindVertexArray.
      *
      * @tparam T The vertex format type
      */
      template <typename T>
      static void SetVertexArrayAttributes();

      /**
      * @brief Creates the vertex array object for a specific vertex format
      *
      * This function should be specialized for each existing vertex format
      *
      * @tparam The vertex format
      *
      * @param[out] vao The created vertex array object, if succeeded
      *
      * @return Was the vertex array object created succesfully?
      */
      template <typename T>
      static bool CreateVertexArray(GLuint* vao);

    public:
      
      /**
      * @brief Creates the vertex buffer from a list of vertices
      *
      * @tparam T The vertex type
      *
      * @param[in] vertices The vertices to create the buffer from
      */
      template <typename T>
      bool Create(const foundation::Vector<T>& vertices);

      /**
      * @brief Sets this vertex buffer as the active vertex buffer, along
      *        with its vertex array object
      */
      void Set();

      /**
      * @return Is this vertex buffer valid for use?
      */
      bool valid() const;

      /**
      * @return The size of the vertices
      */
      size_t size() const;

    protected:

      /**
      * @brief Frees up the GPU memory if allocated, invalidates the buffer
      *
      * @remarks This method is safe to call whether the vertex buffer is
      *          valid or not
      */
      void Release();

    public:

      /**
      * @brief Default destructor
      *
      * @see OGLVertexBuffer::Release
      */
      ~OGLVertexBuffer();

    private:

      GLuint vbo_; //!< The vertex buffer object
      GLuint vao_; //!< The vertex array object

      bool valid_; //!< Is this vertex buffer valid for use?
      size_t size_; //!< The size of the vertices
    };

    //--------------------------------------------------------------------------
    template <typename T>
    inline GLuint OGLVertexBuffer::SetAttributeFloat(
      GLuint i, 
      GLint n, 
      GLint prev)
    {
      glEnableVertexAttribArray(i);
      
      glVertexAttribPointer(
        i, 
        n, 
        GL_FLOAT, 
        GL_FALSE, 
        static_cast<GLsizei>(sizeof(T)), 
        reinterpret_cast<void*>(prev * sizeof(float)));

      OGLUtils::CheckError();

      return prev + n;
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void OGLVertexBuffer::SetVertexArrayAttributes()
    {
      foundation::Logger::Assert(false, "Unspecified OpenGL vertex format");
    }

    //--------------------------------------------------------------------------
    template <>
    inline void OGLVertexBuffer::SetVertexArrayAttributes<Vertex3D>()
    {
      GLuint last = 0;

      last = SetAttributeFloat<Vertex3D>(0, 3, last); // Position
      last = SetAttributeFloat<Vertex3D>(1, 4, last); // Color
      last = SetAttributeFloat<Vertex3D>(2, 3, last); // Normal
      last = SetAttributeFloat<Vertex3D>(3, 3, last); // Tangent
      last = SetAttributeFloat<Vertex3D>(4, 3, last); // Bitangent
      last = SetAttributeFloat<Vertex3D>(5, 2, last); // UV
    }

    //--------------------------------------------------------------------------
    template <>
    inline void OGLVertexBuffer::SetVertexArrayAttributes<Vertex2D>()
    {
      GLuint last = 0;

      last = SetAttributeFloat<Vertex2D>(0, 3, last); // Position
      last = SetAttributeFloat<Vertex2D>(1, 4, last); // Color
      last = SetAttributeFloat<Vertex2D>(2, 2, last); // UV
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline bool OGLVertexBuffer::CreateVertexArray(GLuint* vao)
    {
      if (vao == nullptr)
      {
        return false;
      }

      glGenVertexArrays(1, vao);

      if (OGLUtils::CheckError() == false)
      {
        return false;
      }

      glBindVertexArray(*vao);

      SetVertexArrayAttributes<T>();

      if (OGLUtils::CheckError() == false)
      {
        return false;
      }

      glBindVertexArray(0);

      return true;
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline bool OGLVertexBuffer::Create(const foundation::Vector<T>& vertices)
    {
      Release();

      glGenBuffers(1, &vbo_);

      if (OGLUtils::CheckError() == false)
      {
        return false;
      }

      glBindBuffer(GL_ARRAY_BUFFER, vbo_);

      glBufferData(
        GL_ARRAY_BUFFER, 
        sizeof(T) * vertices.size(), 
        vertices.data(), 
        GL_STATIC_DRAW);

      if (OGLUtils::CheckError() == false)
      {
        Release();
        return false;
      }

      if (CreateVertexArray<T>(&vao_) == false)
      {
        Release();
        return false;
      }

      glBindBuffer(GL_ARRAY_BUFFER, 0);

      valid_ = true;
      size_ = vertices.size();

      return true;
    }
  }
}