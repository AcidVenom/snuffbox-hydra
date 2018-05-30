#include "tools/builder/definitions/asset_types.h"

#include <memory>

namespace snuffbox
{
  namespace builder
  {
    //--------------------------------------------------------------------------
    const char* AssetTypesToBuildExtension(AssetTypes type)
    {
      switch (type)
      {
      case AssetTypes::kScript:
        return "script";
      default:
        return "unknown";
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

      return AssetTypes::kUnknown;
    }

    //--------------------------------------------------------------------------
    const char* AssetTypesToSourceExtension(AssetTypes type)
    {
      switch (type)
      {
      case AssetTypes::kScript:
        return "js";
      default:
        return "unknown";
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

      return AssetTypes::kUnknown;
    }

    //--------------------------------------------------------------------------
    bool BuildToSourceExtension(const char* ext_a, foundation::String* ext_b)
    {
      AssetTypes a = AssetTypesFromBuildExtension(ext_a);
      
      if (a == AssetTypes::kUnknown)
      {
        return false;
      }

      *ext_b = AssetTypesToSourceExtension(a);
      return true;
    }
  }
}