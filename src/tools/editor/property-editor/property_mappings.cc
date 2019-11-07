#include "tools/editor/property-editor/property_mappings.h"

#include <engine/ecs/entity.h>
#include <engine/components/transform_component.h>
#include <engine/components/camera_component.h>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    PropertyMap PropertyMappings::kEntityMapping_;
    PropertyMap PropertyMappings::kComponentsMapping_
      [static_cast<int>(engine::Components::kCount)];

    //--------------------------------------------------------------------------
    void PropertyMappings::InitializeMappings()
    {
      //------------------------------------------------------------------------
      // snuffbox::engine::Entity
      //------------------------------------------------------------------------
      kEntityMapping_ =
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
      kComponentsMapping_[static_cast<int>(engine::Components::kTransform)] =
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

      //------------------------------------------------------------------------
      // snuffbox::engine::CameraComponent
      //------------------------------------------------------------------------
      int camera_index = static_cast<int>(engine::Components::kCamera);
      kComponentsMapping_[camera_index] =
      {
        CreatePropertyPair<EnumProperty, engine::CameraComponent>(
          "projection",
          [](engine::CameraComponent* c, const EnumProperty& value)
          {
            engine::CameraProjection p = 
              static_cast<engine::CameraProjection>(value);

            c->set_projection(p);
          },
          [](engine::CameraComponent* c)
          {
            return static_cast<EnumProperty>(c->projection());
          }
        )
      };

      kComponentsMapping_[camera_index]["projection"]->set_combo_box_values(
      {
        "Perspective",
        "Orthographic"
      });
    }

    //--------------------------------------------------------------------------
    void PropertyMappings::ClearMappings()
    {
      kEntityMapping_.clear();
      
      for (int i = 0; i < static_cast<int>(engine::Components::kCount); ++i)
      {
        kComponentsMapping_[i].clear();
      }
    }

    //--------------------------------------------------------------------------
    const PropertyMap& PropertyMappings::GetEntityMap()
    {
      return kEntityMapping_;
    }

    //--------------------------------------------------------------------------
    const PropertyMap& PropertyMappings::GetComponentMap(
      engine::Components type)
    {
      return kComponentsMapping_[static_cast<int>(type)];
    }
  }
}