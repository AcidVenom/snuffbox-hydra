#pragma once

#include "engine/assets/asset.h"

#include <foundation/containers/vector.h>

namespace snuffbox
{
  namespace engine
  {
    class RendererService;

    /**
    * @brief Used to contain a loaded binary model, that was converted from
    *        the glTF model format
    *
    * @author Daniel Konings
    */
    class ModelAsset : public IAsset
    {

    public:

      /**
      * @brief Used to store per-node data, to re-create the actual model
      *        hierarchy as entities, engine-sided.
      *
      * The scene index is used for access of the GPU handle of the mesh
      */
      struct NodeData
      {
        int index; //!< The scene index of this node in the model
        foundation::Vector<NodeData> children; //!< The children of this node

        /**
        * @brief The transform of the mesh in relation to the root node
        *
        * The root node is created engine-sided, as a top-level entity. From
        * there an entire glTF scene is reconstructed with the correct
        * transformations.
        */
        glm::mat4x4 transform;
      };

      /**
      * @see IAsset::IAsset
      */
      ModelAsset(const foundation::Path& path);

      /**
      * @return Is a mesh at a specified scene index valid?
      *
      * @param[in] scene_index The scene index
      */
      bool IsValid(int scene_index) const;

      /**
      * @return The GPU handle of a specific mesh in the model
      *
      * @return The GPU handle, or nullptr if it is invalid
      */
      void* GetGPUHandle(int scene_index) const;

    protected:

      /**
      * @see IAsset::LoadImpl
      */
      bool LoadImpl(const foundation::Path& path) override;

      /**
      * @see IAsset::UnloadImpl
      */
      void UnloadImpl() override;

      /**
      * @brief Releases the underlying GPU handles
      */
      void Release();

    private:

      RendererService* renderer_; //!< The current renderer service

      /**
      * @brief The GPU handles of each mesh in the model
      *
      * The index of the GPU handle correlates to the scene index of the mesh
      *
      * @see ModelAsset::NodeData
      */
      foundation::Vector<void*> meshes_;

      NodeData root_; //!< The root node of the model
    };
  }
}