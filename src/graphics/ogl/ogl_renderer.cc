#include "graphics/ogl/ogl_renderer.h"
#include "graphics/definitions/shader_constants.h"

#include "graphics/ogl/resources/ogl_mesh.h"
#include "graphics/ogl/resources/ogl_material.h"

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

      if (GLVersion.major < 4 || GLVersion.minor < 6)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kEngine,
          foundation::LogSeverity::kFatal,
          "OpenGL version 4.6 is required for SPIRV support");

        return false;
      }

      glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);

      return OGLUtils::CheckError();
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
    IRendererLoader* OGLRenderer::GetLoader()
    {
      return &loader_;
    }

    //--------------------------------------------------------------------------
    void OGLRenderer::InitializeUBOs()
    {
      PerFrameData pfd;
      memset(&pfd, 0, sizeof(PerFrameData));
      SetFrameData(pfd);

      PerObjectData pod;
      memset(&pod, 0, sizeof(PerObjectData));
      SetFrameData(pod);
    }

    //--------------------------------------------------------------------------
    void OGLRenderer::OnStartFrame()
    {
      per_frame_ubo_.Set(
        ShaderConstants::GetUniformLocation(ShaderUniforms::kPerFrameData));

      per_object_ubo_.Set(
        ShaderConstants::GetUniformLocation(ShaderUniforms::kPerObjectData));
    }

    //--------------------------------------------------------------------------
    void OGLRenderer::SetFrameData(const PerFrameData& pfd)
    {
      per_frame_ubo_.Update(&pfd, sizeof(PerFrameData));
    }

    //--------------------------------------------------------------------------
    void OGLRenderer::SetFrameData(const PerObjectData& pod)
    {
      per_object_ubo_.Update(&pod, sizeof(PerObjectData));
    }

    //--------------------------------------------------------------------------
    void OGLRenderer::DrawMesh(
      IRendererLoader::GPUHandle mesh,
      IRendererLoader::GPUHandle material)
    {
      if (mesh == nullptr)
      {
        //!< @todo Remove this when mesh loading is implemented
        static bool initialized = false;
        static OGLMesh m;

        if (initialized == false)
        {

          glm::vec3 n(0.0f, 0.0f, -1.0f);
          glm::vec3 t(1.0f, 0.0f, 0.0f);
          glm::vec3 b(0.0f, -1.0f, 0.0f);

          foundation::Vector<Vertex3D> verts =
          {
            {{-0.5f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, n, t, b, {0.0f, 0.0f}},
            {{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, n, t, b, {1.0f, 0.0f}},
            {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, n, t, b, {0.0f, 1.0f}},
            {{0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, n, t, b, {1.0f, 1.0f}}
          };

          foundation::Vector<Index> indices = {0, 1, 2, 2, 1, 3};

          m.Create(verts, indices);

          initialized = true;
        }

        mesh = &m;
      }

      OGLMesh* gpu_mesh = reinterpret_cast<OGLMesh*>(mesh);
      OGLMaterial* gpu_mat = reinterpret_cast<OGLMaterial*>(material);

      gpu_mesh->Set();
      gpu_mat->Set();

      glDrawElements(
        GL_TRIANGLES, 
        static_cast<GLsizei>(gpu_mesh->NumIndices()),
        GL_UNSIGNED_INT,
        0);
    }

    //--------------------------------------------------------------------------
    void OGLRenderer::OnResizeImpl(uint16_t width, uint16_t height)
    {

    }
  }
}
