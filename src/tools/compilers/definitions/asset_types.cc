#include "tools/compilers/definitions/asset_types.h"

#include <memory>

namespace snuffbox
{
  namespace compilers
  {
    //--------------------------------------------------------------------------
    const char* AssetTypesToBuildExtension(AssetTypes type)
    {
      switch (type)
      {
      case AssetTypes::kScript:
        return "script";
      case AssetTypes::kScene:
        return "scene";
      case AssetTypes::kVertexShader:
        return "vsb";
      case AssetTypes::kPixelShader:
        return "psb";
      case AssetTypes::kGeometryShader:
        return "gsb";
      case AssetTypes::kMaterial:
        return "mat";

      default:
        break;
      }

      return "unknown";
    }

    //--------------------------------------------------------------------------
    AssetTypes AssetTypesFromBuildExtension(const char* ext)
    {
      AssetTypes current;

      for (int i = 0; i < static_cast<int>(AssetTypes::kCount); ++i)
      { 
        current = static_cast<AssetTypes>(i);
        if (strcmp(ext, AssetTypesToBuildExtension(current)) == 0)
        {
          return current;
        }
      }

      return AssetTypes::kCount;
    }

    //--------------------------------------------------------------------------
    const char* AssetTypesToSourceExtension(AssetTypes type)
    {
      switch (type)
      {
      case AssetTypes::kScript:
        return "js";
      case AssetTypes::kScene:
        return "scene";
      case AssetTypes::kVertexShader:
        return "vs";
      case AssetTypes::kPixelShader:
        return "ps";
      case AssetTypes::kGeometryShader:
        return "gs";
      case AssetTypes::kMaterial:
        return "mat";

      default:
        break;
      }

      return "unknown";
    }

    //--------------------------------------------------------------------------
    AssetTypes AssetTypesFromSourceExtension(const char* ext)
    {
      AssetTypes current;

      for (int i = 0; i < static_cast<int>(AssetTypes::kCount); ++i)
      { 
        current = static_cast<AssetTypes>(i);
        if (strcmp(ext, AssetTypesToSourceExtension(current)) == 0)
        {
          return current;
        }
      }

      return AssetTypes::kCount;
    }

    //--------------------------------------------------------------------------
    bool BuildToSourceExtension(const char* ext_a, foundation::String* ext_b)
    {
      AssetTypes a = AssetTypesFromBuildExtension(ext_a);
      
      if (a == AssetTypes::kCount)
      {
        return false;
      }

      *ext_b = AssetTypesToSourceExtension(a);
      return true;
    }

    //--------------------------------------------------------------------------
    const char* AssetTypesToString(AssetTypes type)
    {
      switch (type)
      {
      case AssetTypes::kScript:
        return "Script";
      case AssetTypes::kScene:
        return "Scene";
      case AssetTypes::kVertexShader:
        return "Vertex Shader";
      case AssetTypes::kPixelShader:
        return "Pixel Shader";
      case AssetTypes::kGeometryShader:
        return "Geometry Shader";
      case AssetTypes::kMaterial:
        return "Material";

      default:
        break;
      }

      return "Unknown Asset";
    }
  }
}
