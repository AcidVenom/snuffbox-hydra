#include "tools/editor/application/editor_application.h"

#include <engine/ecs/entity.h>

#include <engine/services/renderer_service.h>
#include <engine/services/cvar_service.h>
#include <engine/services/scene_service.h>
#include <engine/services/asset_service.h>

#ifndef SNUFF_NSCRIPTING
#include <engine/components/script_component.h>
#include <engine/services/script_service.h>
#endif

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

      InitializeAssets(builder_.build_directory());

      return true;
    }

    //--------------------------------------------------------------------------
    void EditorApplication::SwitchState(States state)
    {
      state_ = state;

      switch (state)
      {
      case States::kPlaying:
        Play();
        break;

      default:
        break;
      }
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

    //--------------------------------------------------------------------------
    void EditorApplication::InitializeAssets(const foundation::Path& build_dir)
    {
      GetService<engine::AssetService>()->Refresh(build_dir);
      builder_.set_on_finished([=](const builder::BuildItem& item)
      {
        OnReload(item);
      });
    }

    //--------------------------------------------------------------------------
    void EditorApplication::OnReload(const builder::BuildItem& item)
    {
      GetService<engine::AssetService>()->
        RegisterAsset(item.type, item.relative);

      if (item.type == builder::AssetTypes::kScript)
      {
        ReloadScriptedEntities();
      }
    }

    //--------------------------------------------------------------------------
    void EditorApplication::ReloadScriptedEntities()
    {
#ifndef SNUFF_NSCRIPTING
      engine::Scene* scene = 
        GetService<engine::SceneService>()->current_scene();

      scene->ForEachEntity([](engine::Entity* ent)
      {
        foundation::Vector<engine::ScriptComponent*> comps =
          ent->GetComponents<engine::ScriptComponent>();

        engine::ScriptComponent* c = nullptr;
        for (size_t i = 0; i < comps.size(); ++i)
        {
          c = comps.at(i);
          c->SetBehavior(c->behavior());
        }
      });
#endif
    }

    //--------------------------------------------------------------------------
    void EditorApplication::Play()
    {
      InitializeAssets(builder_.build_directory());
      RunScripts();
      SCRIPT_CALLBACK(Start);
      GetService<engine::SceneService>()->Start();
    }
  }
}
