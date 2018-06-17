#include "graphics/ogl/ogl_renderer.h"

#include <foundation/auxiliary/logger.h>
#include <glad/glad.h>

namespace snuffbox
{
  namespace graphics
  {
    //--------------------------------------------------------------------------
    OGLRenderer::OGLRenderer(const GraphicsWindow& gw) :
      IRenderer(gw)
    {

    }

    //--------------------------------------------------------------------------
    bool OGLRenderer::Initialize()
    {
      if (context_.Initialize(window().handle) == false)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kEngine,
          foundation::LogSeverity::kFatal,
          "Could not create an OpenGL context");

        return false;
      }

      if (gladLoadGL() <= 0)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kEngine,
          foundation::LogSeverity::kFatal,
          "Could not initialize GLAD");

        return false;
      }

      foundation::Logger::LogVerbosity<2>(
        foundation::LogChannel::kEngine,
        foundation::LogSeverity::kInfo,
        "OpenGL version {0}.{1}",
        GLVersion.major,
        GLVersion.minor);

      return true;
    }

    //--------------------------------------------------------------------------
    void OGLRenderer::SetViewport(const Viewport& vp)
    {
      glViewport(
        static_cast<int>(vp.x), 
        static_cast<int>(vp.y), 
        static_cast<int>(vp.width),
        static_cast<int>(vp.height));
    }

    //--------------------------------------------------------------------------
    void OGLRenderer::Clear(const glm::vec4& color)
    {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glClearColor(color.r, color.g, color.b, color.a);
    }

    //--------------------------------------------------------------------------
    void OGLRenderer::Clear(const RenderTarget& rt, const glm::vec4& color)
    {
      //!< @todo Implement this when render targets are a thing
    }

    //--------------------------------------------------------------------------
    void OGLRenderer::Present(bool vsync)
    {
      context_.Swap(vsync);
    }

    //--------------------------------------------------------------------------
    void OGLRenderer::OnResizeImpl(uint16_t width, uint16_t height)
    {

    }

    //--------------------------------------------------------------------------
    IRendererLoader* OGLRenderer::GetLoader()
    {
      return &loader_;
    }
  }
}
