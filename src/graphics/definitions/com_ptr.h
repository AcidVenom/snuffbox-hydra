#pragma once

#ifndef SNUFF_WIN32
#error "ComPtr is only supported for Windows platforms"
#endif

#include <wrl/client.h>

#ifdef CreateService
#undef CreateService
#endif

namespace snuffbox
{
  namespace graphics
  {
    /**
    * @brief A short-hand for Microsoft's ComPtrs, used by Direct3D SDKs
    */
    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;
  }
}