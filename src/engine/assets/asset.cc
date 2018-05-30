#include "engine/assets/asset.h"

#include <foundation/auxiliary/logger.h>

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    IAsset::IAsset(builder::AssetTypes type, const foundation::Path& path) :
      type_(type),
      path_(path)
    {

    }

    //--------------------------------------------------------------------------
    bool IAsset::Load()
    {
      if (path_.extension() != builder::AssetTypesToExtension(type_))
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kEngine,
          foundation::LogSeverity::kError,
          "Could not load asset '{0}' ({1}) as the extension doesn't match",
          path_,
          static_cast<uint8_t>(type_));

        return false;
      }

      return LoadImpl(path_);
    }

    //--------------------------------------------------------------------------
    builder::AssetTypes IAsset::type() const
    {
      return type_;
    }
  }
}