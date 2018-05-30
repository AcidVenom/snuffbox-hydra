#include "tools/editor/application/editor_application.h"

#include <engine/services/renderer_service.h>
#include <engine/services/cvar_service.h>
#include <engine/services/scene_service.h>

#include <engine/ecs/entity.h>
#include <engine/components/transform_component.h>

#include <engine/assets/script_asset.h>

#include <foundation/auxiliary/timer.h>

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
      window_(nullptr),
      state_(States::kEditing)
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

      window_->CreateHierarchy(GetService<engine::SceneService>());

      window_->BindResizeCallback([&](uint16_t width, uint16_t height)
      {
        renderer->OnResize(width, height);
      });

      window_->show();

      foundation::Timer delta_time("delta_time");
      float dt = 0.0f;

      while (should_quit() == false)
      {
        delta_time.Start();

        processEvents();

        if (state_ == States::kPlaying)
        {
          Update(dt);
        }

        renderer->Render();
        builder_.IdleNotification();

        delta_time.Stop();
        dt = delta_time.Elapsed(foundation::TimeUnits::kSecond);
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
    void EditorApplication::SwitchState(States state)
    {
      state_ = state;
    }

    //--------------------------------------------------------------------------
    EditorApplication::States EditorApplication::state() const
    {
      return state_;
    }

    //--------------------------------------------------------------------------
    void EditorApplication::CreateRenderer()
    {
      CreateService<engine::RendererService>(window_->GetGraphicsWindow());
    }
  }
}
