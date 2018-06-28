#include "graphics/renderer.h"
#include <stdio.h>
namespace snuffbox
{
  namespace graphics
  {
    //--------------------------------------------------------------------------
    const size_t IRenderer::kDefaultQueueSize_ = 4096;

    //--------------------------------------------------------------------------
    IRenderer::IRenderer(const GraphicsWindow& window) :
      window_(window),
      time_(0.0f),
      num_commands_(0)
    {
      static_assert(kDefaultQueueSize_ > 0,
        "The queue size of the renderer must be greater than 0");

      commands_.resize(kDefaultQueueSize_);
    }

    //--------------------------------------------------------------------------
    void IRenderer::Queue(const Camera& camera)
    {
      cameras_.push(camera);
    }

    //--------------------------------------------------------------------------
    void IRenderer::Queue(const DrawCommand& cmd)
    {
      if (num_commands_ >= commands_.size())
      {
        commands_.resize(commands_.size() * 2);
      }

      commands_.at(num_commands_) = cmd;
      ++num_commands_;
    }

    //--------------------------------------------------------------------------
    void IRenderer::Tick(float dt)
    {
      time_ += dt;
    }

    //--------------------------------------------------------------------------
    void IRenderer::Render()
    {
      OnStartFrame();

      while (cameras_.empty() == false)
      {
        Draw(cameras_.front());
        cameras_.pop();
      }

      num_commands_ = 0;
    }

    //--------------------------------------------------------------------------
    void IRenderer::Draw(const Camera& camera)
    {
      PerFrameData pfd;

      pfd.projection = camera.projection;
      pfd.view = camera.view;
      pfd.inv_projection_view = glm::inverse(camera.projection * camera.view);
      pfd.eye_position = camera.eye_position;
      pfd.time = time_;

      SetFrameData(pfd);

      for (size_t i = 0; i < num_commands_; ++i)
      {
        if (commands_.at(i).material == nullptr)
        {
          return;
        }

        //!< @todo Set render targets and such here
        Draw(commands_.at(i), camera);
      }
    }

    //--------------------------------------------------------------------------
    void IRenderer::Draw(const DrawCommand& cmd, const Camera& camera)
    {
      PerObjectData data = cmd.data;
      data.pvw = camera.projection * camera.view * data.world;

      SetFrameData(data);
      DrawMesh(cmd.mesh, cmd.material);
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