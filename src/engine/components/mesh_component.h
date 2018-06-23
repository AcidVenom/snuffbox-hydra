#pragma once

#include "engine/ecs/component.h"

namespace snuffbox
{
  namespace engine
  {
    /**
    * @brief Used for rendering meshes using MeshRendererComponent or as
    *        a definition for mesh colliders
    *
    * @author Daniel Konings
    */
    SCRIPT_CLASS() class MeshComponent :
      public ComponentBase<MeshComponent, Components::kMesh>
    {

    public:

      SCRIPT_NAME(MeshComponent);

      /**
      * @see IComponent::IComponent
      */
      MeshComponent(Entity* entity);

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
    CREATE_COMPONENT(MeshComponent, Components::kMesh);
  }
}