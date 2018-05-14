#pragma once

#include <glm/glm.hpp>
#include <cinttypes>

namespace snuffbox
{
  namespace graphics
  {
    /**
    * @brief A vertex structure to create meshes with used for rendering
    *
    * @author Daniel Konings
    */
    struct Vertex
    {
      glm::vec3 position; //!< The local space position of the vertex
      glm::vec3 normal; //!< The normal of the vertex
      glm::vec2 uv; //!< The UVs of the vertex
    };

    /**
    * @brief A type definition for the indices type used in index buffers
    */
    using Index = uint32_t;
  }
}