#pragma once

#include "engine/ecs/component.h"
#include "engine/assets/asset.h"
#include "engine/graphics/mesh.h"

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
      * @brief Sets the mesh of this MeshComponent
      *
      * @param[in] mesh The mesh to set
      */
      SCRIPT_FUNC() void SetMesh(Mesh* mesh);

      /**
      * @return The mesh asset
      */
      SerializableAsset* asset();

      /**
      * @return The currently set mesh
      */
      SCRIPT_FUNC() Mesh* mesh();

      /**
      * @return The scene index within the model asset
      */
      SCRIPT_FUNC() int scene_index() const;

      /**
      * @see ISerializable::Serialize
      */
      void Serialize(foundation::SaveArchive& archive) const override;

      /**
      * @see ISerializable::Deserialize
      */
      void Deserialize(foundation::LoadArchive& archive) override;

    private:

      SerializableAsset asset_; //!< The model asset
      int scene_index_; //!< The scene index within the model asset

      Mesh mesh_; //!< The currently set mesh
    };

    //--------------------------------------------------------------------------
    CREATE_COMPONENT(MeshComponent, Components::kMesh);
  }
}