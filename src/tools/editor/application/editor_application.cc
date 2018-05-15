#include "tools/editor/application/editor_application.h"

#include <engine/services/renderer_service.h>
#include <engine/services/cvar_service.h>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    EditorApplication::EditorApplication(
      int& argc,
      char** argv,
      const engine::Application::Configuration& cfg)
      :
      Application(argc, argv, cfg),
      QApplication(argc, argv),
      window_(nullptr)
    {

    }

    //--------------------------------------------------------------------------
    foundation::ErrorCodes EditorApplication::Run()
    {
      ApplyConfiguration();

      foundation::Logger::LogVerbosity<2>(
        foundation::LogChannel::kEditor,
        foundation::LogSeverity::kDebug,
        "Initializing the editor"
        );

      window_ = foundation::Memory::ConstructUnique<MainWindow>(
        &foundation::Memory::default_allocator(),
        this);

      foundation::ErrorCodes err = Initialize();

      if (err != foundation::ErrorCodes::kSuccess)
      {
        Shutdown();
        return err;
      }

      engine::RendererService* renderer = GetService<engine::RendererService>();

      window_->BindResizeCallback([&](uint16_t width, uint16_t height)
      {
        renderer->OnResize(width, height);
      });

      window_->show();

      while (should_quit() == false)
      {
        processEvents();
        renderer->Render();
        builder_.IdleNotification();
      }

      Shutdown();

      return foundation::ErrorCodes::kSuccess;
    }

    //--------------------------------------------------------------------------
    bool EditorApplication::SetProjectDirectory(const foundation::Path& path)
    {
      if (foundation::Directory::Exists(path) == false)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kEditor,
          foundation::LogSeverity::kError,
          "Set an invalid project directory: {0}, it does not exist",
          path
          );

        return false;
      }

      if (builder_.Initialize(path) == false)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kEditor,
          foundation::LogSeverity::kError,
          "Could not initialize the builder on project directory '{0}'",
          path
          );

        return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------
    void EditorApplication::CreateRenderer()
    {
      CreateService<engine::RendererService>(window_->GetGraphicsWindow());
    }
  }
}
