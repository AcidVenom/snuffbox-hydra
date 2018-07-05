#pragma once

#include <glm/glm.hpp>
#include <cinttypes>

namespace snuffbox
{
  namespace graphics
  {
    /**
    * @brief A 3D vertex structure to create 3D meshes with used for rendering
    *
    * @author Daniel Konings
    */
    struct Vertex3D
    {
      glm::vec3 position; //!< The local space position of the vertex
      glm::vec4 color; //!< The color of the vertex
      glm::vec3 normal; //!< The normal of the vertex
      glm::vec3 tangent; //!< The tangent of the vertex
      glm::vec2 uv; //!< The UVs of the vertex
    };

    struct Vertex2D
    {
      glm::vec3 position; //!< The local space position of the vertex
      glm::vec4 color; //!< The color of the vertex
      glm::vec2 uv; //!< The UVs of the vertex
    };

    /**
    * @brief A type definition for the indices type used in index buffers
    */
    using Index = uint32_t;
  }
}