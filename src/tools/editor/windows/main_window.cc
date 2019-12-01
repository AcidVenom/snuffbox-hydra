#include "tools/editor/windows/main_window.h"
#include "tools/editor/application/editor_application.h"

#include "tools/editor/editor-widgets/game_view.h"
#include "tools/editor/editor-widgets/console_widget.h"
#include "tools/editor/asset-browser/asset_browser.h"
#include "tools/editor/scene-editor/hierarchy_view.h"
#include "tools/editor/scene-editor/hierarchy_view_item.h"
#include "tools/editor/property-editor/property_view.h"

#include "tools/editor/asset-browser/asset_icon.h"

#include "tools/editor/property-editor/property_mappings.h"

#include <QDockWidget>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QCloseEvent>
#include <QMessageBox>

#define RESET_WINDOW_GEOMETRY 0

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    const int MainWindow::kMinWidth_ = 1280;
    const int MainWindow::kMinHeight_ = 720;

    const QString MainWindow::kSettingsGeometryKey_ = "MainWindow.Geometry";
    const QString MainWindow::kSettingsStateKey_ = "MainWindow.State";
    const QString MainWindow::kSettingsSizeKey_ = "MainWindow.Size";

    //--------------------------------------------------------------------------
    MainWindow::MainWindow(EditorApplication* app, QWidget* parent) :
      QMainWindow(parent),
      app_(app),
      game_view_(nullptr),
      asset_browser_(nullptr),
      properties_(nullptr)
    {
      PropertyMappings::InitializeMappings();

      setObjectName(QStringLiteral("MainWindow"));
      CreateMenuBar();

      setMinimumSize(kMinWidth_, kMinHeight_);
      LoadWindowSize();

      QDockWidget* game_view_widget = new QDockWidget(this);
      game_view_ = new GameView(game_view_widget);
      game_view_widget->setWidget(game_view_);
      game_view_widget->setWindowTitle(QStringLiteral("Game view"));
      game_view_widget->setObjectName(QStringLiteral("MainWindowGameDock"));
      game_view_widget->setMinimumSize(kMinWidth_ * 0.5, kMinHeight_ * 0.5);

      Project& current_project = app->project();
      QString build_path = 
        current_project.GetCurrentBuildPath() + '/' + Project::kAssetDirectory;

      QString asset_path = current_project.GetCurrentAssetsPath();

      QDockWidget* assets_widget = new QDockWidget(this);
      asset_browser_ = 
        new AssetBrowser(this, asset_path, build_path, assets_widget);

      assets_widget->setWidget(asset_browser_);
      assets_widget->setWindowTitle(QStringLiteral("Assets"));
      assets_widget->setObjectName(QStringLiteral("MainWindowAssetsDock"));

      QDockWidget* console_widget = new QDockWidget(this);
      ConsoleWidget* console = new ConsoleWidget(console_widget);
      console_widget->setWidget(console);
      console_widget->setWindowTitle(QStringLiteral("Console"));
      console_widget->setObjectName(QStringLiteral("MainWindowConsoleDock"));

      QDockWidget* scene_widget = new QDockWidget(this);
      HierarchyView* hierarchy = new HierarchyView(app_, scene_widget);
      scene_widget->setWidget(hierarchy);
      scene_widget->setWindowTitle(QStringLiteral("Scene"));
      scene_widget->setObjectName(QStringLiteral("MainWindowSceneDock"));

      QDockWidget* property_widget = new QDockWidget(this);
      properties_ = new PropertyView(hierarchy, property_widget);
      property_widget->setWidget(properties_);
      property_widget->setWindowTitle(QStringLiteral("Properties"));
      property_widget->setObjectName(QStringLiteral("MainWindowPropertyDock"));

      connect(
        app_->asset_importer(),
        &AssetImporter::SceneChanged,
        hierarchy,
        &HierarchyView::OnSceneChanged);

      connect(
        this,
        &MainWindow::Undone,
        hierarchy,
        &HierarchyView::Undo);

      connect(
        this,
        &MainWindow::Redone,
        hierarchy,
        &HierarchyView::Redo);

      connect(
        this,
        &MainWindow::SceneRefresh,
        hierarchy,
        &HierarchyView::OnSceneRefreshed);

      connect(
        hierarchy,
        &HierarchyView::ItemSelectionChanged,
        properties_,
        [this](HierarchyViewItem* item)
        {
          properties_->ShowForEntity(
            item == nullptr ? nullptr : item->entity());
        });

      addDockWidget(Qt::DockWidgetArea::TopDockWidgetArea, scene_widget);
      addDockWidget(Qt::DockWidgetArea::TopDockWidgetArea, game_view_widget);
      addDockWidget(Qt::DockWidgetArea::TopDockWidgetArea, property_widget);
      addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea, assets_widget);
      addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea, console_widget);

      LoadWindowGeometry();

      foundation::Logger::RedirectOutput(&ConsoleWidget::OnLog, console);

      RefreshAssetList();
    }

    //--------------------------------------------------------------------------
    void MainWindow::CreateMenuBar()
    {
      QMenu* file_menu = menuBar()->addMenu("File");
      QMenu* edit_menu = menuBar()->addMenu("Edit");

      QAction* open_project = new QAction("Open project");
      open_project->setShortcut(Qt::CTRL + Qt::Key_O);

      QAction* new_scene = new QAction("New scene");
      new_scene->setShortcut(Qt::CTRL + Qt::Key_N);

      QAction* save_scene = new QAction("Save scene");
      save_scene->setShortcut(Qt::CTRL + Qt::Key_S);

      QAction* save_scene_as = new QAction("Save scene as");
      save_scene_as->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_S);

      QAction* exit_app = new QAction("Exit");
      exit_app->setShortcut(Qt::CTRL + Qt::Key_Q);

      connect(open_project, &QAction::triggered, this, [&]()
      {
        OnOpenProject();
      });

      connect(new_scene, &QAction::triggered, this, [&]()
      {
        OnNewScene();
      });

      connect(save_scene, &QAction::triggered, this, [&]()
      {
        OnSaveScene();
      });

      connect(save_scene_as, &QAction::triggered, this, [&]()
      {
        OnSaveSceneAs();
      });

      connect(exit_app, &QAction::triggered, this, [&]()
      {
        ConfirmExit();
      });

      QAction* undo = new QAction("Undo");
      undo->setShortcut(Qt::CTRL + Qt::Key_Z);

      QAction* redo = new QAction("Redo");
      redo->setShortcut(Qt::CTRL + Qt::Key_Y);

      connect(undo, &QAction::triggered, this, [&]()
      {
        emit Undone();
      });

      connect(redo, &QAction::triggered, this, [&]()
      {
        emit Redone();
      });

      file_menu->addAction(open_project);
      file_menu->addSeparator();
      file_menu->addAction(new_scene);
      file_menu->addAction(save_scene);
      file_menu->addAction(save_scene_as);
      file_menu->addSeparator();
      file_menu->addAction(exit_app);

      edit_menu->addAction(undo);
      edit_menu->addAction(redo);
    }

    //--------------------------------------------------------------------------
    void MainWindow::RefreshAssetList()
    {
      asset_browser_->Refresh();
    }

    //--------------------------------------------------------------------------
    GameView* MainWindow::game_view() const
    {
      return game_view_;
    }

    //--------------------------------------------------------------------------
    void MainWindow::SaveWindowGeometry()
    {
      QSettings& settings = EditorApplication::Instance()->GlobalSettings();
      settings.setValue(kSettingsGeometryKey_, saveGeometry());
      settings.setValue(kSettingsStateKey_, saveState());
      settings.setValue(kSettingsSizeKey_, size());

      asset_browser_->SaveState(&settings);
    }

    //--------------------------------------------------------------------------
    void MainWindow::LoadWindowGeometry()
    {
#if RESET_WINDOW_GEOMETRY
      return;
#endif
      QSettings& settings = EditorApplication::Instance()->GlobalSettings();

      if (
        settings.contains(kSettingsGeometryKey_) == true &&
        settings.contains(kSettingsStateKey_) == true)
      {
        restoreGeometry(settings.value(kSettingsGeometryKey_).toByteArray());
        restoreState(settings.value(kSettingsStateKey_).toByteArray());
      }

      asset_browser_->LoadState(&settings);
    }
    //--------------------------------------------------------------------------
    void MainWindow::LoadWindowSize()
    {
#if RESET_WINDOW_GEOMETRY
      return;
#endif
      QSettings& settings = EditorApplication::Instance()->GlobalSettings();

      if (settings.contains(kSettingsSizeKey_) == true)
      {
        resize(settings.value(kSettingsSizeKey_).toSize());
      }
    }

    //--------------------------------------------------------------------------
    void MainWindow::OnOpenProject()
    {
      app_->TryOpenProject();
    }

    //--------------------------------------------------------------------------
    void MainWindow::OnNewScene()
    {
      QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "New scene",
        "Do you want to save the changes in the current scene?",
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

      if (reply == QMessageBox::Cancel)
      {
        return;
      }

      emit SceneRefresh();

      if (reply == QMessageBox::Yes)
      {
        app_->asset_importer()->SaveCurrentScene();
      }

      app_->asset_importer()->NewScene();
    }

    //--------------------------------------------------------------------------
    void MainWindow::OnSaveScene()
    {
      emit SceneRefresh();
      app_->asset_importer()->SaveCurrentScene();
    }

    //--------------------------------------------------------------------------
    void MainWindow::OnSaveSceneAs()
    {
      emit SceneRefresh();
      app_->asset_importer()->SaveCurrentScene(true);
    }

    //--------------------------------------------------------------------------
    bool MainWindow::ConfirmExit()
    {
      QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Close",
        "Are you sure you want to close the application?"
        "\nAny unsaved data will be lost",
        QMessageBox::Yes | QMessageBox::No);

      bool accepted = reply == QMessageBox::Yes;
      if (accepted == true)
      {
        app_->NotifyQuit();
      }

      return accepted;
    }

    //--------------------------------------------------------------------------
    void MainWindow::closeEvent(QCloseEvent* evt)
    {
      if (ConfirmExit() == false)
      {
        evt->ignore();
        return;
      }

      SaveWindowGeometry();
    }

    //--------------------------------------------------------------------------
    void MainWindow::OnAssetImported(
      compilers::AssetTypes type, 
      const QString& full_path)
    {
      app_->asset_importer()->ImportAsset(type, full_path.toLatin1().data());
    }

    //--------------------------------------------------------------------------
    void MainWindow::OnUpdate()
    {
      properties_->Update();
    }

    //--------------------------------------------------------------------------
    bool MainWindow::IsResizing() const
    {
      return game_view_->IsResizing();
    }

    //--------------------------------------------------------------------------
    MainWindow::~MainWindow()
    {
      foundation::Logger::RedirectOutput(nullptr, nullptr);

      PropertyMappings::ClearMappings();
    }
  }
}