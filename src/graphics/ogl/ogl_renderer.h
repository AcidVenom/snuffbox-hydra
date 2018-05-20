#pragma once

#include "graphics/renderer.h"
#include "graphics/ogl/ogl_context.h"

namespace snuffbox
{
  namespace graphics
  {
    /**
    * @brief The OpenGL renderer for Snuffbox, which can be bound to a
    *        specified graphics window and is the main renderer on Linux
    *
    * @author Daniel Konings
    */
    class OGLRenderer : public IRenderer
    {

    public:

      /**
      * @see IRenderer::IRenderer
      */
      OGLRenderer(const GraphicsWindow& gw);

      /**
      * @see IRenderer::Initialize
      */
      bool Initialize() override;

      /**
      * @see IRenderer::SetViewport
      */
      void SetViewport(const Viewport& vp) override;

      /**
      * @see IRenderer::Clear
      */
      void Clear(const glm::vec4& color) override;

      /**
      * @see IRenderer::Clear
      */
      void Clear(const RenderTarget& rt, const glm::vec4& color) override;

      /**
      * @see IRenderer::Present
      */
      void Present(bool vsync) override;

    protected:

      /**
      * @see IRenderer::OnResizeImpl
      */
      void OnResizeImpl(uint16_t width, uint16_t height) override;

    private:

      OGLContext context_; //!< The OpenGL context
    };
  }
}