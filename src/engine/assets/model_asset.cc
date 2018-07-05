#include "engine/assets/model_asset.h"

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    ModelAsset::ModelAsset(const foundation::Path& path) :
      IAsset(compilers::AssetTypes::kModel, path)
    {

    }

    //--------------------------------------------------------------------------
    bool ModelAsset::LoadImpl(const foundation::Path& path)
    {
      return true;
    }

    //--------------------------------------------------------------------------
    void ModelAsset::UnloadImpl()
    {

    }
  }
}