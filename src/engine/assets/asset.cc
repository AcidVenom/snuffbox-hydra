#include "engine/assets/asset.h"

#include <foundation/auxiliary/logger.h>

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    IAsset::IAsset(compilers::AssetTypes type, const foundation::Path& path) :
      type_(type),
      path_(path),
      is_loaded_(false)
    {

    }

    //--------------------------------------------------------------------------
    bool IAsset::Load()
    {
      if (is_loaded_ == true)
      {
        Unload();
        return Load();
      }

      if (path_.extension() != compilers::AssetTypesToBuildExtension(type_))
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kEngine,
          foundation::LogSeverity::kError,
          "Could not load asset '{0}' ({1}) as the extension doesn't match",
          path_,
          AssetTypesToString(type_));

        return false;
      }

      is_loaded_ = LoadImpl(path_);
      return is_loaded_;
    }

    //--------------------------------------------------------------------------
    void IAsset::Unload()
    {
      if (is_loaded_ == false)
      {
        return;
      }

      UnloadImpl();
      is_loaded_ = false;
    }

    //--------------------------------------------------------------------------
    compilers::AssetTypes IAsset::type() const
    {
      return type_;
    }

    //--------------------------------------------------------------------------
    const foundation::Path& IAsset::path() const
    {
      return path_;
    }

    //--------------------------------------------------------------------------
    bool IAsset::is_loaded() const
    {
      return is_loaded_;
    }

    //--------------------------------------------------------------------------
    IAsset::~IAsset()
    {

    }
  }
}