#pragma once

#include "engine/assets/asset.h"

#include <tools/compilers/compilers/model_compiler.h>

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
      * @see IAsset::IAsset
      */
      ModelAsset(const foundation::Path& path, const foundation::String& name);

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

      /**
      * @brief Instantiates this model asset as an entity
      */
      void Instantiate();

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
      * @brief A short-hand to reach the nodes of the current model compiler
      */
      using CompilerNode = compilers::ModelCompiler<graphics::Vertex3D>::Node;

      /**
      * @brief The compiled nodes, to reconstruct the model engine-sided
      */
      foundation::Vector<CompilerNode> nodes_;

      /**
      * @brief The GPU handles of each mesh in the model
      *
      * The index of the GPU handle correlates to the scene index of the mesh
      *
      * @see ModelAsset::NodeData
      */
      foundation::Vector<void*> meshes_;
    };
  }
}