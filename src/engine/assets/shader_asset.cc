#include "engine/assets/shader_asset.h"

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    ShaderAsset::ShaderAsset(
      const foundation::Path& path,
      compilers::AssetTypes type)
      :
      IAsset(type, path)
    {

    }

    //--------------------------------------------------------------------------
    bool ShaderAsset::LoadImpl(const foundation::Path& path)
    {
      return true;
    }

    void ShaderAsset::UnloadImpl()
    {

    }
  }
}