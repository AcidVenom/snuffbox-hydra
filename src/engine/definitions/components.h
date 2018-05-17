#pragma once

#include <scripting/script_class.h>

namespace snuffbox
{
  namespace engine
  {
    /**
    * @brief The list of component types as a enumeration
    */
    SCRIPT_ENUM() enum class Components
    {
      kTransform,
      kScript,
      kMeshRenderer,
      kCount
    };
  }
}

SCRIPT_ENUM_DECL(snuffbox::engine::Components);