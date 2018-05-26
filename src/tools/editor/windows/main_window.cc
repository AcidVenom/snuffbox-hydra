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
      project_dir_(""),
      on_resize_(nullptr)
    {
      ui_.setupUi(this);

      ApplyStyle(app);
      CreateConsole();
      CreateInspector();

      BindEvents();

      LoadLayout();
    }

    //--------------------------------------------------------------------------
    void MainWindow::ApplyStyle(QApplication* app)
    {
      app->setStyle(QStyleFactory::create("Fusion"));
      app->setPalette(EditorColors::DefaultPalette());

      const QColor& dock = EditorColors::DockColor();
      int r, g, b;
      dock.getRgb(&r, &g, &b);

      std::string dock_col = 
        "rgb(" 
        + std::to_string(r) + ", " 
        + std::to_string(g) + ", " 
        + std::to_string(b) + ")";

      QString qdock_col = dock_col.c_str();

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
    }

    //--------------------------------------------------------------------------
    void MainWindow::CreateInspector()
    {
      inspector_ = foundation::Memory::ConstructUnique<Inspector>(
        &foundation::Memory::default_allocator(),
        ui_.inspectorTree);
    }

    //--------------------------------------------------------------------------
    void MainWindow::LoadLayout()
    {
      resizeDocks({ ui_.consoleDock }, { 100 }, Qt::Vertical);

      QSettings settings(kCompanyName_, kAppName_);

      restoreGeometry(settings.value(kSaveGeometry_).toByteArray());
      restoreState(settings.value(kSaveWindow_).toByteArray());

      ui_.splitter->restoreState(
        settings.value(kSaveSplitterA_).toByteArray());

      ui_.splitter_2->restoreState(
        settings.value(kSaveSplitterB_).toByteArray());
    }

    //--------------------------------------------------------------------------
    void MainWindow::SaveLayout()
    {
      QSettings settings(kCompanyName_, kAppName_);

      settings.setValue(kSaveGeometry_, saveGeometry());
      settings.setValue(kSaveWindow_, saveState());

      settings.setValue(kSaveSplitterA_, ui_.splitter->saveState());
      settings.setValue(kSaveSplitterB_, ui_.splitter_2->saveState());
    }

    //--------------------------------------------------------------------------
    void MainWindow::OpenProject()
    {
      QString dir = QFileDialog::getExistingDirectory(
        this, 
        "Open Project",
        project_dir_,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

      if (app_->SetProjectDirectory(dir.toStdString().c_str()) == true)
      {
        project_dir_ = dir;
      }
    }

    //--------------------------------------------------------------------------
    void MainWindow::OnSelectEntity(engine::Entity* entity)
    {
      inspector_->ShowEntity(entity);
    }

    //--------------------------------------------------------------------------
    EditorApplication* MainWindow::app() const
    {
      return app_;
    }
  }
}
