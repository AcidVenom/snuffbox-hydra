#pragma once

#include <scripting/script_class.h>

namespace snuffbox
{
  namespace engine
  {
    /**
    * @brief The different camera projection modes
    */
    SCRIPT_ENUM() enum class CameraProjection
    {
      kPerspective,
      kOrthographic
    };
  }
}

SCRIPT_ENUM_DECL(snuffbox::engine::CameraProjection);