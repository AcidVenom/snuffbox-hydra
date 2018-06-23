#pragma once

#include "engine/ecs/component.h"

namespace snuffbox
{
  namespace engine
  {
    class MaterialAsset;

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
      * @brief Sets the material of this component
      *
      * @param[in] path The path to the material to set
      */
      SCRIPT_FUNC() void SetMaterial(const foundation::String& path);

      /**
      * @return The material path of this component
      */
      SCRIPT_FUNC() const foundation::String& material_path() const;

      /**
      * @return The material to use for rendering
      */
      MaterialAsset* material() const;

      /**
      * @see ISerializable::Serialize
      */
      void Serialize(foundation::SaveArchive& archive) const override;

      /**
      * @see ISerializable::Deserialize
      */
      void Deserialize(foundation::LoadArchive& archive) override;

    private:

      foundation::String material_path_; //!< The current material path
      MaterialAsset* material_; //!< The material to use for rendering
    };

    //--------------------------------------------------------------------------
    CREATE_COMPONENT(MeshRendererComponent, Components::kMeshRenderer);
  }
}