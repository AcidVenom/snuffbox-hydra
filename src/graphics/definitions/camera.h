#pragma once

#include "graphics/definitions/render_target.h"

#include <glm/glm.hpp>

namespace snuffbox
{
  namespace graphics
  {
    /**
    * @brief Used to contain camera matrices to set to the renderer
    *
    * @author Daniel Konings
    */
    struct Camera
    {
      glm::mat4x4 view; //!< The view matrix
      glm::mat4x4 projection; //!< The projection matrix
      glm::vec3 eye_position; //!< The camera's eye position

      /**
      * @brief The render targets that this camera will bind
      */
      RenderTarget render_targets[RenderTarget::kMaxBoundTargets];
      unsigned int num_targets; //!< The number of render targets
    };
  }
}