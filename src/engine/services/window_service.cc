#include "engine/services/window_service.h"
#include "engine/application/application.h"

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    WindowService::WindowService() :
      ServiceBase<WindowService>("WindowService")
    {

    }

    //--------------------------------------------------------------------------
    foundation::ErrorCodes WindowService::OnInitialize(Application& app)
    {
      if (Window::InitializeGLFW() == false)
      {
        return foundation::ErrorCodes::kGLFWInitializationFailed;
      }

      const Application::Configuration& cfg = app.config();

      window_ = foundation::Memory::ConstructUnique<Window>(
        &foundation::Memory::default_allocator(),
        cfg.application_name,
        cfg.window_width,
        cfg.window_height
        );

      if (window_->Initialize() == false)
      {
        window_.reset();
        return foundation::ErrorCodes::kCreateWindowError;
      }

      return foundation::ErrorCodes::kSuccess;
    }

    //--------------------------------------------------------------------------
    void WindowService::OnShutdown(Application& app)
    {
      window_.reset();
      Window::ShutdownGLFW();
    }

    //--------------------------------------------------------------------------
    bool WindowService::ProcessEvents()
    {
      return window_ == nullptr ? true : window_->ProcessEvents();
    }
  }
}
