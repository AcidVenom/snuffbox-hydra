#include "engine/services/renderer_service.h"
#include "engine/services/cvar_service.h"

#include <foundation/auxiliary/logger.h>

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    const bool RendererService::kDefaultVsync_ = true;
    const double RendererService::kDefaultWidth_ = 1280.0;
    const double RendererService::kDefaultHeight_ = 720.0;

    //--------------------------------------------------------------------------
    RendererService::RendererService(const graphics::GraphicsWindow& gw) :
      ServiceBase<RendererService>("RendererService"),
      cvar_(nullptr)
    {
      renderer_ = foundation::Memory::ConstructUnique<graphics::Renderer>(
        &foundation::Memory::default_allocator(),
        gw);
    }

    //--------------------------------------------------------------------------
    void RendererService::RegisterCVars(CVarService* cvar)
    {
      cvar->Register("r_vsync", "Should VSync be enabled?", kDefaultVsync_);
      cvar->Register("r_width", "Renderer width in pixels", kDefaultWidth_);
      cvar->Register("r_height", "Renderer height in pixels", kDefaultHeight_);

      cvar_ = cvar;
    }

    //--------------------------------------------------------------------------
    void RendererService::Render(float dt)
    {
      if (renderer_ == nullptr)
      {
        return;
      }

      bool sync = cvar_->Get<bool>("r_vsync");
      float w = cvar_->Get<float>("r_width");
      float h = cvar_->Get<float>("r_height");

      renderer_->Tick(dt);
      renderer_->SetViewport(graphics::Viewport{ 0.0f, 0.0f, w, h });
      renderer_->Clear(glm::vec4{ 0.0f, 0.5, 1.0f, 1.0f });
      renderer_->Present(sync);
    }

    //--------------------------------------------------------------------------
    graphics::IRendererLoader* RendererService::GetLoader()
    {
      return renderer_->GetLoader();
    }

    //--------------------------------------------------------------------------
    void RendererService::OnResize(uint16_t width, uint16_t height)
    {
      foundation::Logger::LogVerbosity<3>(
        foundation::LogChannel::kEngine,
        foundation::LogSeverity::kDebug,
        "Resized the window to {0}x{1}",
        width,
        height
        );

      renderer_->OnResize(width, height);
    }

    //--------------------------------------------------------------------------
    foundation::ErrorCodes RendererService::OnInitialize(Application& app)
    {
      if (renderer_->Initialize() == false)
      {
        return foundation::ErrorCodes::kRendererInitializationFailed;
      }

      return foundation::ErrorCodes::kSuccess;
    }

    //--------------------------------------------------------------------------
    void RendererService::OnShutdown(Application& app)
    {

    }
  }
}
