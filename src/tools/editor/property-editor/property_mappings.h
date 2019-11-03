#pragma once

#include "tools/editor/property-editor/property_value.h"

#include <engine/definitions/components.h>

namespace snuffbox
{
  namespace editor
  {
    /**
    * @brief Used to initialize all the valid properties for a specific type
    *
    * @author Daniel Konings
    */
    struct PropertyMappings
    {
      /**
      * @brief The mappings of properties for an entity
      */
      static PropertyMap kEntityMapping;

      /**
      * @brief The mappings of properties for each type of component
      */
      static PropertyMap kComponentsMapping
        [static_cast<int>(engine::Components::kCount)];

      /**
      * @brief Initializes the mappings, ready for use
      */
      static void InitializeMappings();

      /**
      * @brief Clears all mappings initialized in 
      *        PropertyMappings::InitializeMappings
      */
      static void ClearMappings();
    };
  }
}