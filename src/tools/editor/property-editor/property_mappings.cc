#include "tools/editor/property-editor/property_mappings.h"

#include <engine/ecs/entity.h>
#include <engine/components/transform_component.h>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    PropertyMap PropertyMappings::kEntityMapping;
    PropertyMap PropertyMappings::kComponentsMapping
      [static_cast<int>(engine::Components::kCount)];

    //--------------------------------------------------------------------------
    void PropertyMappings::InitializeMappings()
    {
      //------------------------------------------------------------------------
      // snuffbox::engine::Entity
      //------------------------------------------------------------------------
      kEntityMapping =
      {
        CreatePropertyPair<foundation::String, engine::Entity>(
          "name",
          [](engine::Entity* entity, const foundation::String& value)
          {
            entity->set_name(value);
          },
          [](engine::Entity* entity)
          {
            return entity->name();
          }
        ),

        CreatePropertyPair<bool, engine::Entity>(
          "active",
          [](engine::Entity* entity, const bool& value)
          {
            entity->set_active(value);
          },
          [](engine::Entity* entity)
          {
            return entity->active();
          }
        ),
      };

      //------------------------------------------------------------------------
      // snuffbox::engine::TransformComponent
      //------------------------------------------------------------------------
      kComponentsMapping[static_cast<int>(engine::Components::kTransform)] =
      {
        CreatePropertyPair<glm::vec3, engine::TransformComponent>(
          "position",
          [](engine::TransformComponent* t, const glm::vec3& value)
          {
            t->SetLocalPosition(value);
          },
          [](engine::TransformComponent* t)
          {
            return t->GetLocalPosition();
          }
        ),

        CreatePropertyPair<glm::vec3, engine::TransformComponent>(
          "rotation",
          [](engine::TransformComponent* t, const glm::vec3& value)
          {
            t->SetRotationEuler(value);
          },
          [](engine::TransformComponent* t)
          {
            return t->GetRotationEuler();
          }
        ),

        CreatePropertyPair<glm::vec3, engine::TransformComponent>(
          "scale",
          [](engine::TransformComponent* t, const glm::vec3& value)
          {
            t->SetScale(value);
          },
          [](engine::TransformComponent* t)
          {
            return t->GetScale();
          }
        ),
      };
    }

    //--------------------------------------------------------------------------
    void PropertyMappings::ClearMappings()
    {
      kEntityMapping.clear();
      kComponentsMapping->clear();
    }
  }
}