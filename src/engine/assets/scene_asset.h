#pragma once

#include "engine/assets/asset.h"

namespace snuffbox
{
  namespace engine
  {
    class Scene;

    /**
    * @brief Used to load scene assets from raw JSON serialized data
    *
    * @see SceneCompiler
    *
    * @author Daniel Konings
    */
    class SceneAsset : public IAsset
    {

    public:

      /**
      * @see IAsset::IAsset
      */
      SceneAsset(const foundation::Path& path, const foundation::String& name);

      /**
      * @return The scene contained by this asset
      */
      Scene* scene() const;

    protected:

      /**
      * @see IAsset::LoadImpl
      */
      bool LoadImpl(const foundation::Path& path) override;

      /**
      * @see IAsset::UnloadImpl
      */
      void UnloadImpl() override;

    public:

      /**
      * @brief Destructs the created scene
      */
      ~SceneAsset();

    private:

      Scene* scene_; //!< The scene contained by this asset
    };
  }
}