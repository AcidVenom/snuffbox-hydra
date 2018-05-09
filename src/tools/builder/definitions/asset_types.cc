#include "tools/builder/definitions/asset_types.h"

namespace snuffbox
{
  namespace builder
  {
    //--------------------------------------------------------------------------
    const char* AssetTypesToExtension(AssetTypes type)
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
  }
}