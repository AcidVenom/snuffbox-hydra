#include "tools/editor/application/editor_application.h"
#include "tools/editor/windows/main_window.h"

#include "tools/editor/editor-widgets/game_view.h"

#include "tools/editor/project/project_window.h"
#include "tools/editor/application/styling.h"

#include "tools/builder/builder.h"

#ifndef SNUFF_NSCRIPTING
#include <engine/services/script_service.h>
#endif

#include <engine/services/renderer_service.h>
#include <engine/services/asset_service.h>

#include <foundation/auxiliary/logger.h>
#include <foundation/auxiliary/timer.h>

#include <QApplication>
#include <memory>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    const int EditorApplication::kMinBuildChangeWait_ = 200;
    const QString EditorApplication::kTitleFormat_ =
      "Snuffbox Editor - %0";

    //--------------------------------------------------------------------------
    EditorApplication::EditorApplication(
      int argc, char** argv, const Configuration& cfg)
      :
      engine::Application(argc, argv, cfg),
      qapp_(argc, argv),
      project_(nullptr),
      build_change_timer_("BuildChangeTimer"),
      build_dir_changed_(false),
      main_window_(nullptr),
      asset_importer_(nullptr),
      project_changed_(false)
    {
      QCoreApplication::setOrganizationName(
        QStringLiteral("Daniel Konings"));

      QCoreApplication::setOrganizationDomain(
        QStringLiteral("www.danielkonings.com"));

      QCoreApplication::setApplicationName(cfg.application_name);

      Styling::ApplyStyle(&qapp_);
    }

    //--------------------------------------------------------------------------
    EditorApplication* EditorApplication::Instance()
    {
      return static_cast<EditorApplication*>(Application::Instance());
    }

    //--------------------------------------------------------------------------
    foundation::ErrorCodes EditorApplication::Run()
    {
      SetAsInstance();

      bool was_restarted = project_changed_ == true;

      if (was_restarted)
      {
        OnRestart();
      }

      project_changed_ = false;

      foundation::ErrorCodes err = foundation::ErrorCodes::kSuccess;
      if (was_restarted == false && ShowProjectWindow() == false)
      {
        return err;
      }

      ApplyConfiguration();

      if ((err = Initialize()) != foundation::ErrorCodes::kSuccess)
      {
        return err;
      }

      asset_importer_ = std::unique_ptr<AssetImporter>(new AssetImporter(this));

      main_window_ = CreateMainWindow(&err);
      if (err != foundation::ErrorCodes::kSuccess)
      {
        return err;
      }

      builder::Builder builder;
      if (builder.Initialize(
        project_.project_path().toLatin1().data(),
        Project::kAssetDirectory,
        Project::kBuildDirectory,
        [&](const builder::BuildItem& item)
        {
          OnBuilderFinished(&builder, item);
        },
        [&](const builder::BuildItem& item)
        {
          OnBuilderChanged(&builder, item);
        }
      ) == false)
      {
        return foundation::ErrorCodes::kBuilderInitializationFailed;
      }

      GetService<engine::AssetService>()->Refresh(builder.build_directory());

      engine::RendererService* renderer = GetService<engine::RendererService>();

      foundation::Timer delta_time("delta_time");
      float dt = 0.0f;

      while (should_quit() == false && main_window_->isVisible() == true)
      {
        delta_time.Start();

        qapp_.processEvents();
        renderer->Render(dt);

        delta_time.Stop();
        dt = delta_time.Elapsed(foundation::TimeUnits::kSecond);

        builder.IdleNotification();

        CheckForBuildChanges();
      }

      builder.Shutdown();
      Shutdown();

      main_window_.reset();
      asset_importer_.reset();

      return project_changed_ ? 
        foundation::ErrorCodes::kRestart : foundation::ErrorCodes::kSuccess;
    }

    //--------------------------------------------------------------------------
    void EditorApplication::TryOpenProject()
    {
      if (ShowProjectWindow() == true)
      {
        project_changed_ = true;
        NotifyQuit();
      }
    }

    //--------------------------------------------------------------------------
    QSettings& EditorApplication::GlobalSettings() const
    {
      const Configuration& cfg = config();

      static QSettings settings(
        QStringLiteral("Daniel Konings"), 
        cfg.application_name);

      return settings;
    }

    //--------------------------------------------------------------------------
    void EditorApplication::SetSceneInWindowTitle(const QString& scene_name)
    {
      QString title = kTitleFormat_.arg(scene_name);
      qapp_.setApplicationDisplayName(title);

      if (main_window_ != nullptr)
      {
        main_window_->setWindowTitle(title);
      }
    }

    //--------------------------------------------------------------------------
    AssetImporter* EditorApplication::asset_importer() const
    {
      return asset_importer_.get();
    }

    //--------------------------------------------------------------------------
    Project& EditorApplication::project()
    {
      return project_;
    }

    //--------------------------------------------------------------------------
    void EditorApplication::OnRestart()
    {
      should_quit_ = false;
    }

    //--------------------------------------------------------------------------
    bool EditorApplication::ShowProjectWindow()
    {
      std::unique_ptr<ProjectWindow>
        project_window(new ProjectWindow(&project_));

      project_window->show();

      if (project_window->exec() == QDialog::Rejected)
      {
        return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------
    std::unique_ptr<MainWindow> EditorApplication::CreateMainWindow(
      foundation::ErrorCodes* err)
    {
      std::unique_ptr<MainWindow> main_window(new MainWindow(this));

      foundation::Logger::LogVerbosity<2>(
        foundation::LogChannel::kEditor,
        foundation::LogSeverity::kDebug,
        "Initializing the editor\n\tOpening project: {0}",
        project_.project_path().toLatin1().data());

      GameView* game_view = main_window->game_view();

      if (*err == foundation::ErrorCodes::kSuccess)
      {
        *err = CreateRenderer(game_view->GetGraphicsWindow());
      }

      if (*err != foundation::ErrorCodes::kSuccess)
      {
        Shutdown();
        return nullptr;
      }

      main_window->show();

      engine::RendererService* renderer = GetService<engine::RendererService>();
      game_view->BindResizeCallback([renderer](uint16_t w, uint16_t h)
      {
        renderer->OnResize(w, h);
      });

      return main_window;
    }

    //--------------------------------------------------------------------------
    void EditorApplication::OnBuilderFinished(
      const builder::Builder* builder, 
      const builder::BuildItem& item)
    {
      engine::AssetService* as = GetService<engine::AssetService>();
      as->RegisterAsset(item.relative);

      foundation::String asset_path = item.relative.NoExtension().ToString();
      as->Reload(item.type, asset_path);

      if (builder->IsBuilding() == true)
      {
        return;
      }

      BuildChanged();
    }

    //--------------------------------------------------------------------------
    void EditorApplication::OnBuilderChanged(
      const builder::Builder* builder, 
      const builder::BuildItem& item)
    {
      BuildChanged();
    }

    //--------------------------------------------------------------------------
    void EditorApplication::BuildChanged()
    {
      std::unique_lock<std::mutex> lock_guard(build_mutex_);

      build_change_timer_.Stop();
      build_change_timer_.Start();

      build_dir_changed_ = true;
    }

    //--------------------------------------------------------------------------
    void EditorApplication::CheckForBuildChanges()
    {
      if (
        build_change_timer_.Elapsed() > kMinBuildChangeWait_ &&
        build_dir_changed_ == true)
      {
        build_dir_changed_ = false;
        main_window_->RefreshAssetList();

        foundation::Logger::LogVerbosity<3>(
          foundation::LogChannel::kBuilder,
          foundation::LogSeverity::kDebug,
          "Build directory changed");
      }
    }
  }
}