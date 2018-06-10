#include "tools/editor/windows/main_window.h"
#include "tools/editor/windows/console.h"
#include "tools/editor/application/editor_application.h"
#include "tools/editor/definitions/editor_colors.h"

#include <qstylefactory.h>
#include <qevent.h>
#include <qfiledialog.h>
#include <qsettings.h>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    const char* MainWindow::kCompanyName_ = "Snuffbox";
    const char* MainWindow::kAppName_ = "snuffbox-hydra-editor";
    const char* MainWindow::kSaveGeometry_ = "geometry";
    const char* MainWindow::kSaveWindow_ = "window";
    const char* MainWindow::kSaveSplitterA_ = "splitter_a";
    const char* MainWindow::kSaveSplitterB_ = "splitter_b";

    //--------------------------------------------------------------------------
    MainWindow::MainWindow(EditorApplication* app) :
      app_(app),
      console_(nullptr),
      hierarchy_(nullptr),
      inspector_(nullptr),
      asset_browser_(nullptr),
      project_dir_(""),
      current_scene_("New scene"),
      on_resize_(nullptr)
    {
      ui_.setupUi(this);

      ApplyStyle(app);

      CreateConsole();
      CreateInspector();
      CreateAssetBrowser();

      BindEvents();

      LoadLayout();
    }

    //--------------------------------------------------------------------------
    void MainWindow::ApplyStyle(QApplication* app)
    {
      app->setStyle(QStyleFactory::create("Fusion"));
      app->setPalette(EditorColors::DefaultPalette());

      QString qdock_col = EditorColors::ColorToCSS(EditorColors::DockColor());

      app->setStyleSheet("                      \
        QMainWindow::separator                  \
        {                                       \
          background-color: " + qdock_col + ";  \
        }                                       \
        QDockWidget::title                      \
        {                                       \
          background-color: " + qdock_col + ";  \
          border: 1px solid rgb(88, 106, 80);   \
        }                                       \
        QDockWidget                             \
        {                                       \
          color: white;                         \
        }"
      );
    }

    //--------------------------------------------------------------------------
    void MainWindow::closeEvent(QCloseEvent* evt)
    {
      app_->NotifyQuit();
      SaveLayout();

      evt->accept();
    }

    //--------------------------------------------------------------------------
    bool MainWindow::eventFilter(QObject* obj, QEvent* evt)
    {
      if (evt->type() == QEvent::Paint)
      {
        if (on_resize_ != nullptr)
        {
          on_resize_(ui_.gameWindow->width(), ui_.gameWindow->height());
        }

        return true;
      }

      return false;
    }

    //--------------------------------------------------------------------------
    graphics::GraphicsWindow MainWindow::GetGraphicsWindow() const
    {
      graphics::GraphicsWindow gw;
      gw.width = ui_.gameWindow->width();
      gw.height = ui_.gameWindow->height();
      gw.handle = reinterpret_cast<void*>(ui_.gameWindow->winId());

      return gw;
    }

    //--------------------------------------------------------------------------
    void MainWindow::BindResizeCallback(
      const graphics::GraphicsWindow::SizeCallback& cb)
    {
      on_resize_ = cb;
    }

    //--------------------------------------------------------------------------
    void MainWindow::BindEvents()
    {
      connect(
        ui_.actionOpen_Project,
        &QAction::triggered,
        this,
        &MainWindow::OpenProject);

      connect(
        ui_.actionNew_Scene,
        &QAction::triggered,
        this,
        &MainWindow::NewScene);

      connect(
        ui_.actionSave_Scene,
        &QAction::triggered,
        this,
        &MainWindow::SaveScene);

      connect(
        ui_.actionSave_Scene_As,
        &QAction::triggered,
        this,
        &MainWindow::SaveSceneAs);

      connect(
        ui_.playButton,
        &QPushButton::pressed,
        this,
        &MainWindow::OnPlay);

      ui_.gameWindow->installEventFilter(this);
    }

    //--------------------------------------------------------------------------
    void MainWindow::CreateConsole()
    {
      QTextBrowser* output_windows[] =
      {
        ui_.allOutput,
        ui_.engineOutput,
        ui_.editorOutput,
        ui_.playerOutput,
        ui_.scriptOutput,
        ui_.builderOutput
      };

      console_ = foundation::Memory::ConstructUnique<Console>(
        &foundation::Memory::default_allocator(),
        ui_.outputTabs,
        output_windows);
    }

    //--------------------------------------------------------------------------
    void MainWindow::CreateHierarchy(engine::SceneService* scene_service)
    {
      hierarchy_ = foundation::Memory::ConstructUnique<HierarchyView>(
        &foundation::Memory::default_allocator(),
        ui_.hierarchyView,
        scene_service);

      connect(
        hierarchy_.get(),
        SIGNAL(SelectEntity(engine::Entity*)),
        this,
        SLOT(OnSelectEntity(engine::Entity*)));

      connect(
        inspector_.get(),
        SIGNAL(RefreshHierarchy()),
        hierarchy_.get(),
        SLOT(OnHierarchyChanged()));

      connect(
        this,
        SIGNAL(SceneChanged()),
        hierarchy_.get(),
        SLOT(OnHierarchyChanged()));
    }

    //--------------------------------------------------------------------------
    void MainWindow::CreateInspector()
    {
      inspector_ = foundation::Memory::ConstructUnique<Inspector>(
        &foundation::Memory::default_allocator(),
        ui_.inspectorTree);
    }

    //--------------------------------------------------------------------------
    void MainWindow::CreateAssetBrowser()
    {
      asset_browser_ = foundation::Memory::ConstructUnique<AssetBrowser>(
        &foundation::Memory::default_allocator(),
        ui_.buildDirectoryBrowser,
        ui_.assetLayout);

      connect(
        asset_browser_.get(),
        SIGNAL(DoubleClickedAsset(QString, int)),
        this,
        SLOT(OnDoubleClickedAsset(QString, int)));
    }

    //--------------------------------------------------------------------------
    void MainWindow::LoadLayout()
    {
#ifdef SNUFF_WIN32
      /**
      * @brief On Windows, the splitter within the console dock messes up
      *        after the dock is resized, this is a workaround to "fix" that
      *        issue
      */
      resizeDocks({ ui_.consoleDock }, { 100 }, Qt::Vertical);
#endif

      QSettings settings(kCompanyName_, kAppName_);

      restoreGeometry(settings.value(kSaveGeometry_).toByteArray());
      restoreState(settings.value(kSaveWindow_).toByteArray());

      ui_.assetSplitter->restoreState(
        settings.value(kSaveSplitterA_).toByteArray());

      ui_.consoleSplitter->restoreState(
        settings.value(kSaveSplitterB_).toByteArray());
    }

    //--------------------------------------------------------------------------
    void MainWindow::SaveLayout()
    {
      QSettings settings(kCompanyName_, kAppName_);

      settings.setValue(kSaveGeometry_, saveGeometry());
      settings.setValue(kSaveWindow_, saveState());

      settings.setValue(kSaveSplitterA_, ui_.assetSplitter->saveState());
      settings.setValue(kSaveSplitterB_, ui_.consoleSplitter->saveState());
    }

    //--------------------------------------------------------------------------
    void MainWindow::MarkPlaybackButton(QPushButton* button, bool enabled)
    {
      const QString css = EditorColors::ColorToCSS(EditorColors::BlueButton());
      const QString sheet_e = "QPushButton{ background-color: " + css + "; }";
      const QString sheet = "QPushButton{}";

      button->setStyleSheet(enabled == true ? sheet_e : sheet);
    }

    //--------------------------------------------------------------------------
    void MainWindow::SetPlaybackEnabled(bool enabled)
    {
      ui_.playButton->setEnabled(enabled);
    }

    //--------------------------------------------------------------------------
    void MainWindow::RefreshWindowTitle()
    {
      setWindowTitle(
        "Snuffbox Editor - " + current_scene_ + " (" + project_dir_ + ")");
    }

    //--------------------------------------------------------------------------
    void MainWindow::OpenProject()
    {
      QString dir = QFileDialog::getExistingDirectory(
        this, 
        "Open Project",
        project_dir_,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

      if (dir.size() == 0)
      {
        return;
      }

      if (app_->SetProjectDirectory(dir.toStdString().c_str()) == true)
      {
        project_dir_ = dir;
        std::string str = (project_dir_ + "/.build").toStdString();
        asset_browser_->Refresh(str.c_str());

        RefreshWindowTitle();
      }
    }

    //--------------------------------------------------------------------------
    void MainWindow::NewScene()
    {
      if (app_->NewScene() == true)
      {
        current_scene_ = "New scene";
      }

      RefreshWindowTitle();
    }

    //--------------------------------------------------------------------------
    void MainWindow::SaveScene()
    {
      if (app_->SaveCurrentScene() == true)
      {
        current_scene_ = app_->GetLoadedScene();
      }

      RefreshWindowTitle();
    }

    //--------------------------------------------------------------------------
    void MainWindow::SaveSceneAs()
    {
      if (app_->SaveCurrentScene(true) == true)
      {
        current_scene_ = app_->GetLoadedScene();
      }

      RefreshWindowTitle();
    }

    //--------------------------------------------------------------------------
    void MainWindow::OnSelectEntity(engine::Entity* entity)
    {
      inspector_->ShowEntity(entity);
    }

    //--------------------------------------------------------------------------
    void MainWindow::OnPlay()
    {
      EditorApplication::States state = app_->state();

      EditorApplication::States next = 
        state == EditorApplication::States::kEditing ?
      EditorApplication::States::kPlaying : EditorApplication::States::kEditing;

      bool enabled = next == EditorApplication::States::kPlaying;

      ui_.pauseButton->setEnabled(enabled);
      ui_.speedDownButton->setEnabled(enabled);
      ui_.speedUpButton->setEnabled(enabled);

      MarkPlaybackButton(ui_.playButton, enabled);
      app_->SwitchState(next);
    }

    //--------------------------------------------------------------------------
    void MainWindow::OnDoubleClickedAsset(QString relative, int type)
    {
      compilers::AssetTypes t = static_cast<compilers::AssetTypes>(type);

      switch (t)
      {
      case compilers::AssetTypes::kScene:
        app_->OpenScene(relative.toStdString().c_str());
        break;

      default:
        break;
      }

      current_scene_ = app_->GetLoadedScene();
      RefreshWindowTitle();
    }

    //--------------------------------------------------------------------------
    EditorApplication* MainWindow::app() const
    {
      return app_;
    }
  }
}
