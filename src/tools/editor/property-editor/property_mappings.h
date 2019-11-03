#pragma once

#include "tools/editor/property-editor/property_value.h"

#include <engine/definitions/components.h>

namespace snuffbox
{
  namespace editor
  {
    struct PropertyMappings
    {
      static PropertyMap kEntityMapping;

      static PropertyMap kComponentsMapping
        [static_cast<int>(engine::Components::kCount)];

      static void InitializeMappings();
      static void ClearMappings();
    };
  }
}