#pragma once

namespace snuffbox
{
  namespace builder
  {
    /**
    * @brief The different asset types that can be built by the builder and
    *        read by the engine
    *
    * The asset types should be in the order of dependencies. For instance,
    * a material depends on a shader and a texture, so the material enum
    * value should always be later than both of these.
    *
    * @see BuildScheduler
    */
    enum class AssetTypes
    {
      kScript,
      kCount,
      kUnknown = kCount,
    };
  }
}