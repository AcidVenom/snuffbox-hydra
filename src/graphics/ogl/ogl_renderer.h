#pragma once

#include "graphics/renderer.h"
#include "graphics/ogl/ogl_context.h"
#include "graphics/ogl/ogl_loader.h"

#include "graphics/ogl/resources/ogl_uniform_buffer.h"

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

      /**
      * @see IRenderer::GetLoader
      */
      IRendererLoader* GetLoader() override;

    protected:

      /**
      * @brief Initializes the default UBOs for use by memsetting them to 0
      */
      void InitializeUBOs();

      /**
      * @see IRenderer::OnStartFrame
      */
      void OnStartFrame() override;

      /**
      * @see IRenderer::SetFrameData
      */
      void SetFrameData(const PerFrameData& pfd) override;

      /**
      * @see IRenderer::SetFrameData
      */
      void SetFrameData(const PerObjectData& pod) override;

      /**
      * @see IRenderer::DrawMesh
      */
      void DrawMesh(
        IRendererLoader::GPUHandle mesh, 
        IRendererLoader::GPUHandle material) override;

      /**
      * @see IRenderer::OnResizeImpl
      */
      void OnResizeImpl(uint16_t width, uint16_t height) override;

    private:

      OGLContext context_; //!< The OpenGL context
      OGLLoader loader_; //!< The renderer loader
      OGLUniformBuffer per_frame_ubo_; //!< The per-frame uniform buffer
      OGLUniformBuffer per_object_ubo_; //!< The per-object uniform buffer
    };
  }
}
