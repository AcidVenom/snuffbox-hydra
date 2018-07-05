#pragma once

#include "engine/assets/asset.h"

namespace snuffbox
{
  namespace engine
  {
    class ModelAsset : public IAsset
    {

    public:

      /**
      * @see IAsset::IAsset
      */
      ModelAsset(const foundation::Path& path);

    protected:

      /**
      * @see IAsset::LoadImpl
      */
      bool LoadImpl(const foundation::Path& path) override;

      /**
      * @see IAsset::UnloadImpl
      */
      void UnloadImpl() override;
    };
  }
}