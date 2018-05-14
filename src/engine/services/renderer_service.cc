#include "engine/services/renderer_service.h"
#include "engine/services/cvar_service.h"

#include <foundation/auxiliary/logger.h>

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    RendererService::RendererService(
      const graphics::GraphicsWindow& gw,
      CVarService* cvar) 
      :
      ServiceBase<RendererService>("RendererService"),
      cvar_(cvar)
    {
      renderer_ = foundation::Memory::ConstructUnique<graphics::Renderer>(
        &foundation::Memory::default_allocator(),
        gw);
    }

    //--------------------------------------------------------------------------
    void RendererService::RegisterCVars()
    {
      cvar_->Register("r_vsync", "Should VSync be enabled?", true);
      cvar_->Register("r_width", "The rendering width in pixels", 1280.0);
      cvar_->Register("r_height", "The rendering height in pixels", 720.0);
    }

    //--------------------------------------------------------------------------
    void RendererService::Present()
    {
      if (renderer_ == nullptr)
      {
        return;
      }

      bool vsync = static_cast<CVar<bool>*>(cvar_->Get("r_vsync"))->value();

      double width 
        = static_cast<CVar<double>*>(cvar_->Get("r_width"))->value();

      double height 
        = static_cast<CVar<double>*>(cvar_->Get("r_height"))->value();

      float fwidth = static_cast<float>(width);
      float fheight = static_cast<float>(height);

      renderer_->SetViewport(graphics::Viewport{0.0f, 0.0f, fwidth, fheight});
      renderer_->Clear(glm::vec4{0.0f, 0.5, 1.0f, 1.0f});
      renderer_->Present(vsync);
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