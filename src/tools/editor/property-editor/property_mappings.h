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

    public:

      /**
      * @brief Initializes the mappings, ready for use
      */
      static void InitializeMappings();

      /**
      * @brief Clears all mappings initialized in 
      *        PropertyMappings::InitializeMappings
      */
      static void ClearMappings();

      /**
      * @return An entity's property map
      */
      static const PropertyMap& GetEntityMap();

      /**
      * @brief Retrieves the property map of a specific component type
      *
      * @param[in] type The type of the component to retrieve
      *
      * @return The retrieved property map
      */
      static const PropertyMap& GetComponentMap(engine::Components type);

    private:

      /**
      * @brief The mappings of properties for an entity
      */
      static PropertyMap kEntityMapping_;

      /**
      * @brief The mappings of properties for each type of component
      */
      static PropertyMap kComponentsMapping_
        [static_cast<int>(engine::Components::kCount)];
    };
  }
}