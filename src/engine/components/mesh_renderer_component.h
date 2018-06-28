#pragma once

#include "engine/ecs/component.h"
#include "engine/assets/asset.h"

namespace snuffbox
{
  namespace engine
  {
    class MaterialAsset;
    class RendererService;

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
      * @see IComponent::Update
      *
      * Updates the renderer component to send a render queue message to
      * the RendererService
      */
      void Update(float dt) override;

      /**
      * @brief Sets a material of this component
      *
      * @param[in] idx The index of the material to set
      * @param[in] material The material asset to set
      */
      SCRIPT_FUNC() void SetMaterial(int idx, IAsset* material);

      /**
      * @brief Retrieves a material of this component
      *
      * @param[in] idx The index of the material to retrieve
      *
      * @return The material, or nullptr if it does not exist
      */
      SCRIPT_FUNC() IAsset* GetMaterial(int idx);

      /**
      * @return The full list of serializable assets
      */
      foundation::Vector<SerializableAsset>& materials();

      /**
      * @see ISerializable::Serialize
      */
      void Serialize(foundation::SaveArchive& archive) const override;

      /**
      * @see ISerializable::Deserialize
      */
      void Deserialize(foundation::LoadArchive& archive) override;

    private:

      /**
      * @brief The materials per sub-mesh
      */
      foundation::Vector<SerializableAsset> materials_;
      RendererService* renderer_; //!< The renderer service

      static const int kMaxMaterials_; //!< The maximum number of materials
    };

    //--------------------------------------------------------------------------
    CREATE_COMPONENT(MeshRendererComponent, Components::kMeshRenderer);
  }
}