#include "engine/services/renderer_service.h"

#include <foundation/auxiliary/logger.h>

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    RendererService::RendererService(const graphics::GraphicsWindow& gw) :
      ServiceBase<RendererService>("RendererService")
    {
      renderer_ = foundation::Memory::ConstructUnique<graphics::Renderer>(
        &foundation::Memory::default_allocator(),
        gw);
    }

    //--------------------------------------------------------------------------
    void RendererService::Present()
    {
      if (renderer_ == nullptr)
      {
        return;
      }

      renderer_->Present();
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