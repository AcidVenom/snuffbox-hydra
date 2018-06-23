#pragma once

#include "engine/ecs/component.h"

namespace snuffbox
{
  namespace engine
  {
    /**
    * @brief Used to render meshes within the scene, based on an entity's
    *        MeshComponent
    *
    * @see MeshComponent
    *
    * @author Daniel Konings
    */
    SCRIPT_CLASS() class MeshRendererComponent :
      public ComponentBase<MeshRendererComponent, Components::kMeshRenderer>
    {

    public:

      SCRIPT_NAME(MeshRendererComponent);

      /**
      * @see IComponent::IComponent
      */
      MeshRendererComponent(Entity* entity);

      /**
      * @see ISerializable::Serialize
      */
      void Serialize(foundation::SaveArchive& archive) const override;

      /**
      * @see ISerializable::Deserialize
      */
      void Deserialize(foundation::LoadArchive& archive) override;
    };

    //--------------------------------------------------------------------------
    CREATE_COMPONENT(MeshRendererComponent, Components::kMeshRenderer);
  }
}