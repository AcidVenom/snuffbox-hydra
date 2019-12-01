#include "tools/editor/property-editor/property_mappings.h"

#include <engine/ecs/entity.h>

#include <engine/components/transform_component.h>
#include <engine/components/camera_component.h>
#include <engine/components/script_component.h>
#include <engine/components/mesh_component.h>
#include <engine/components/mesh_renderer_component.h>

#include <engine/assets/model_asset.h>

#undef near
#undef far

#define CREATE_MATERIAL_PROP(id, i)\
{\
PropertyPair pair =\
CreatePropertyPair<engine::SerializableAsset, engine::MeshRendererComponent>(\
#i ":material_" #i,\
[](engine::MeshRendererComponent* m, const engine::SerializableAsset& value)\
{\
  engine::Material mat(value.handle);\
  m->SetMaterial(i, &mat);\
},\
[](engine::MeshRendererComponent* m)\
{\
  return m->GetMaterial(i)->GetSerializableAsset();\
});\
kComponentsMapping_[id].emplace(pair);\
}

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
          "0:name",
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
          "1:active",
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
          "0:position",
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
          "1:rotation",
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
          "2:scale",
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
      // snuffbox::engine::ScriptComponent
      //------------------------------------------------------------------------
      kComponentsMapping_[static_cast<int>(engine::Components::kScript)] =
      {
        CreatePropertyPair<foundation::String, engine::ScriptComponent>(
          "0:behavior",
          [](engine::ScriptComponent* s, const foundation::String& value)
          {
            s->SetBehavior(value);
          },
          [](engine::ScriptComponent* s)
          {
            return s->behavior();
          }
        )
      };

      //------------------------------------------------------------------------
      // snuffbox::engine::MeshComponent
      //------------------------------------------------------------------------
      kComponentsMapping_[static_cast<int>(engine::Components::kMesh)] =
      {
        CreatePropertyPair<engine::SerializableAsset, engine::MeshComponent>(
          "0:mesh",
          [](engine::MeshComponent* m, const engine::SerializableAsset& value)
          {
            engine::Mesh mesh(value.handle, m->scene_index());
            m->SetMesh(&mesh);
          },
          [](engine::MeshComponent* m)
          {
            return *m->asset();
          }
        ),

        CreatePropertyPair<double, engine::MeshComponent>(
          "1:scene_index",
          [](engine::MeshComponent* m, const double& value)
          {
            engine::Mesh mesh(m->asset()->handle, static_cast<int>(value));
            m->SetMesh(&mesh);
          },
          [](engine::MeshComponent* m)
          {
            return static_cast<double>(m->scene_index());
          }
        )
      };

      //------------------------------------------------------------------------
      // snuffbox::engine::MeshRendererComponent
      //------------------------------------------------------------------------
      int mrc_id = static_cast<int>(engine::Components::kMeshRenderer);
      kComponentsMapping_[mrc_id] = {};

      CREATE_MATERIAL_PROP(mrc_id, 0);
      CREATE_MATERIAL_PROP(mrc_id, 1);
      CREATE_MATERIAL_PROP(mrc_id, 2);
      CREATE_MATERIAL_PROP(mrc_id, 3);
      CREATE_MATERIAL_PROP(mrc_id, 4);
      CREATE_MATERIAL_PROP(mrc_id, 5);
      CREATE_MATERIAL_PROP(mrc_id, 6);
      CREATE_MATERIAL_PROP(mrc_id, 7);

      //------------------------------------------------------------------------
      // snuffbox::engine::CameraComponent
      //------------------------------------------------------------------------
      int camera_index = static_cast<int>(engine::Components::kCamera);
      kComponentsMapping_[camera_index] =
      {
        CreatePropertyPair<double, engine::CameraComponent>(
          "0:near",
          [](engine::CameraComponent* c, const double& value)
          {
            c->set_near(static_cast<float>(value));
          },
          [](engine::CameraComponent* c)
          {
            return static_cast<double>(c->near());
          }
        ),

        CreatePropertyPair<double, engine::CameraComponent>(
          "1:far",
          [](engine::CameraComponent* c, const double& value)
          {
            c->set_far(static_cast<float>(value));
          },
          [](engine::CameraComponent* c)
          {
            return static_cast<double>(c->far());
          }
        ),

        CreatePropertyPair<double, engine::CameraComponent>(
          "2:fov",
          [](engine::CameraComponent* c, const double& value)
          {
            c->set_fov(static_cast<float>(value));
          },
          [](engine::CameraComponent* c)
          {
            return static_cast<double>(c->fov());
          }
        ),

        CreatePropertyPair<double, engine::CameraComponent>(
          "3:aspect",
          [](engine::CameraComponent* c, const double& value)
          {
            c->set_aspect(static_cast<float>(value));
          },
          [](engine::CameraComponent* c)
          {
            return static_cast<double>(c->aspect());
          }
        ),

        CreatePropertyPair<double, engine::CameraComponent>(
          "4:orthographic_size",
          [](engine::CameraComponent* c, const double& value)
          {
            c->set_orthographic_size(static_cast<float>(value));
          },
          [](engine::CameraComponent* c)
          {
            return static_cast<double>(c->orthographic_size());
          }
        ),

        CreatePropertyPair<EnumProperty, engine::CameraComponent>(
          "5:projection",
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

      kComponentsMapping_[camera_index]["5:projection"]->set_combo_box_values(
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