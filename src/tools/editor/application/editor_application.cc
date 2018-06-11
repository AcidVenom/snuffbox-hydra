#include "tools/editor/application/editor_application.h"

#include <engine/ecs/entity.h>

#include <engine/services/renderer_service.h>
#include <engine/services/cvar_service.h>
#include <engine/services/scene_service.h>
#include <engine/services/asset_service.h>
#include <engine/assets/scene_asset.h>

#ifndef SNUFF_NSCRIPTING
#include <engine/components/script_component.h>
#include <engine/services/script_service.h>
#endif

#include <foundation/auxiliary/timer.h>
#include <foundation/serialization/save_archive.h>
#include <foundation/io/file.h>

#if defined(SNUFF_LINUX) && defined(Unsorted)
#undef Unsorted
#endif

#include <qfiledialog.h>
#include <qmessagebox.h>

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
      project_dir_(""),
      assets_dir_(""),
      loaded_scene_(""),
      state_(States::kUninitialized),
      has_error_(false)
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

      SwitchState(States::kEditing);

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

      project_dir_ = path.ToString().c_str();
      assets_dir_ = project_dir_ + "/assets";

      InitializeAssets(builder_.build_directory());

      return true;
    }

    //--------------------------------------------------------------------------
    void EditorApplication::SwitchState(States state)
    {
      state_ = state;

      switch (state)
      {
      case States::kEditing:
        OnStartEditing();
        break;

      case States::kPlaying:
        Play();
        break;

      default:
        break;
      }
    }

    //--------------------------------------------------------------------------
    bool EditorApplication::NewScene()
    {
      if (ShowSceneSaveDialog() == false)
      {
        return false;
      }

      GetService<engine::SceneService>()->SwitchScene(nullptr);
      loaded_scene_ = "";

      emit window_->SceneChanged();

      return true;
    }

    //--------------------------------------------------------------------------
    void EditorApplication::OpenScene(const foundation::Path& path)
    {
      if (loaded_scene_ == path.ToString())
      {
        return;
      }

      if (ShowSceneSaveDialog() == false)
      {
        return;
      }

      loaded_scene_ = path.ToString();

      foundation::String asset = path.NoExtension().ToString();
      GetService<engine::SceneService>()->SwitchScene(asset);

      emit window_->SceneChanged();
    }

    //--------------------------------------------------------------------------
    bool EditorApplication::SaveCurrentScene(bool dialog)
    {
      engine::Scene* scene =
        GetService<engine::SceneService>()->current_scene();

      bool show_dialog = true;
      QString path = "";

      if (loaded_scene_.size() > 0)
      {
        foundation::Path dir = assets_dir_.toStdString().c_str();
        dir = dir / loaded_scene_;

        if (foundation::File::Exists(dir) == true)
        {
          path = dir.ToString().c_str();
          show_dialog = false;
        }
      }

      if (dialog == true || show_dialog == true)
      {
        path = QFileDialog::getSaveFileName(window_.get(),
         "Save scene", assets_dir_, "Scene files (*.scene)");
      }

      if (path.size() > 0)
      {
        foundation::SaveArchive archive;
        archive(scene);

        std::string s_path = path.toStdString();
        foundation::Path full_path = s_path.c_str();

        if (archive.WriteFile(full_path) == false)
        {
          foundation::Logger::LogVerbosity<1>(
            foundation::LogChannel::kEditor,
            foundation::LogSeverity::kError,
            "Could not save scene file '{0}'",
            full_path);

          return false;
        }

        std::string assets_path = assets_dir_.toStdString();

        loaded_scene_ = full_path.StripPath(
          foundation::Path(assets_path.c_str())).ToString();

        return true;
      }

      return false;
    }

    //--------------------------------------------------------------------------
    QString EditorApplication::GetLoadedScene()
    {
      return loaded_scene_.c_str();
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
      ReloadScripts();

      builder_.set_on_finished([=](const builder::BuildItem& item)
      {
        OnReload(item);
      });

      builder_.set_on_removed([=](const builder::BuildItem& item)
      {
        OnRemoved(item);
      });
    }

    //--------------------------------------------------------------------------
    void EditorApplication::OnReload(const builder::BuildItem& item)
    {
      engine::AssetService* as = GetService<engine::AssetService>();

      compilers::AssetTypes type = item.type;
      const foundation::Path& relative = item.relative;

      as->RegisterAsset(type, relative);

      if (type == compilers::AssetTypes::kScript)
      {
        ReloadScripts();
        return;
      }

      foundation::String path = relative.NoExtension().ToString();
      if (as->IsLoaded(type, path) == true)
      {
        as->Load(type, path);
      }
    }

    //--------------------------------------------------------------------------
    void EditorApplication::OnRemoved(const builder::BuildItem& item)
    {
      engine::AssetService* as = GetService<engine::AssetService>();

      compilers::AssetTypes type = item.type;
      const foundation::Path& relative = item.relative;

      as->RemoveAsset(type, relative);
    }

    //--------------------------------------------------------------------------
    void EditorApplication::ReloadScripts()
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

        return true;
      });

      engine::AssetService* a = GetService<engine::AssetService>();

      if (a->LoadAll(compilers::AssetTypes::kScript) == false)
      {
        has_error_ = true;

        if (state_ == States::kEditing)
        {
          foundation::Logger::LogVerbosity<1>(
            foundation::LogChannel::kEditor,
            foundation::LogSeverity::kError,
            "Please fix script compilation errors before entering play mode");

          window_->SetPlaybackEnabled(false);
        }

        return;
      }

      window_->SetPlaybackEnabled(true);
      has_error_ = false;
#endif
    }

    //--------------------------------------------------------------------------
    void EditorApplication::OnStartEditing()
    {
      if (has_error_ == true)
      {
        window_->SetPlaybackEnabled(false);
      }
    }

    //--------------------------------------------------------------------------
    void EditorApplication::Play()
    {
      RunScripts();
      SCRIPT_CALLBACK(Start);
      GetService<engine::SceneService>()->Start();
    }

    //--------------------------------------------------------------------------
    bool EditorApplication::ShowSceneSaveDialog()
    {
      QMessageBox::StandardButton reply;

      reply = QMessageBox::question(window_.get(), 
        "Save current scene?",
        "Do you want to save the current scene?",
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

      if (reply == QMessageBox::Yes)
      {
        SaveCurrentScene();
      } 
      else if (reply == QMessageBox::Cancel)
      {
        return false;
      }

      return true;
    }
  }
}
