#include "graphics/renderer.h"

namespace snuffbox
{
  namespace graphics
  {
    //--------------------------------------------------------------------------
    IRenderer::IRenderer(const GraphicsWindow& window) :
      window_(window)
    {

    }

    //--------------------------------------------------------------------------
    void IRenderer::OnResize(uint16_t width, uint16_t height)
    {
      window_.width = width;
      window_.height = height;

      OnResizeImpl(width, height);
    }

    //--------------------------------------------------------------------------
    const GraphicsWindow& IRenderer::window() const
    {
      return window_;
    }

    //--------------------------------------------------------------------------
    IRenderer::~IRenderer()
    {

    }
  }
}