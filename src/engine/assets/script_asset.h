#pragma once

#include "engine/assets/asset.h"

namespace snuffbox
{
  namespace engine
  {
    /**
    * @brief Used to load script assets from RC4 encrypted raw strings
    *
    * @see ScriptCompiler
    *
    * @author Daniel Konings
    */
    class ScriptAsset : public IAsset
    {

    public:

      /**
      * @see IAsset::IAsset
      */
      ScriptAsset(const foundation::Path& path);

    protected:

      /**
      * @see IAsset::Load
      */
      bool LoadImpl(const foundation::Path& path) override;
    };
  }
}