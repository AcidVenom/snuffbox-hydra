#pragma once

#ifdef SNUFF_D3D11
#include "graphics/d3d11/d3d11_renderer.h"
namespace snuffbox
{
  namespace graphics
  {
    using Renderer = D3D11Renderer;
  }
}
#elif defined(SNUFF_D3D12)
#include "graphics/d3d12/d3d12_renderer.h"
namespace snuffbox
{
  namespace graphics
  {
    using Renderer = D3D12Renderer;
  }
}
#elif defined (SNUFF_OGL)
#include "graphics/ogl/ogl_renderer.h"
namespace snuffbox
{
  namespace graphics
  {
    using Renderer = OGLRenderer;
  }
}
#else
#error "Unsupported renderer implementation"
#endif