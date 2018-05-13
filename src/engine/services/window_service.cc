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

    //--------------------------------------------------------------------------
    void WindowService::Show()
    {
      window_->Show();
    }

    //--------------------------------------------------------------------------
    Window* WindowService::GetWindow() const
    {
      if (window_ == nullptr)
      {
        return nullptr;
      }

      return window_.get();
    }

    //--------------------------------------------------------------------------
    graphics::GraphicsWindow WindowService::GetGraphicsWindow() const
    {
      graphics::GraphicsWindow gw;
      gw.width = window_->width_;
      gw.height = window_->height_;
      gw.handle = window_->NativeHandle();

      return gw;
    }

    //--------------------------------------------------------------------------
    void WindowService::BindResizeCallback(
      const graphics::GraphicsWindow::SizeCallback& cb)
    {
      window_->on_resize_ = cb;
    }
  }
}
