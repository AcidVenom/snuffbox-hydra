#pragma once

#ifndef EASTL_USER_CONFIG_HEADER
#define EASTL_USER_CONFIG_HEADER "foundation/memory/eastl_config.h"
#endif

#include <EASTL/functional.h>

namespace snuffbox
{
  namespace foundation
  {
    /**
    * @brief An EASTL function
    */
    template <typename T>
    using Function = eastl::function<T>;
  }
}