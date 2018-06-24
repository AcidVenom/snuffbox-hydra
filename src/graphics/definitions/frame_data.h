#pragma once

#include <glm/glm.hpp>

namespace snuffbox
{
  namespace graphics
  {
    /**
    * @brief The per frame data to be set at the start of a frame
    *
    * @author Daniel Konings
    */
    struct PerFrameData
    {
      glm::mat4x4 projection; //!< The current camera's projection matrix
      glm::mat4x4 view; //!< The current camera's view matrix
      glm::mat4x4 inv_projection_view; //!< The inverse of the projection * view
      glm::vec3 eye_position; //!< The eye position of the camera
      float time; //!< The currently elapsed application time
    };

    /**
    * @brief The data to be set per rendered object
    *
    * @author Daniel Konings
    */
    struct PerObjectData
    {
      glm::mat4x4 world; //!< The world matrix
      glm::mat4x4 pvw; //!< The projection * view * world matrix
      glm::mat4x4 inv_world; //!< The inverse world matrix
    };
  }
}