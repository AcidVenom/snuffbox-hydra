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
      * @brief The GPU handle of each mesh in the model
      */
      foundation::Vector<void*> gpu_handles_;
    };
  }
}