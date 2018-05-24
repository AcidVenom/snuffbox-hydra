#include "tools/editor/windows/main_window.h"
#include "tools/editor/windows/console.h"
#include "tools/editor/application/editor_application.h"

#include <engine/ecs/scene.h>
#include <engine/ecs/entity.h>
#include <engine/components/transform_component.h>

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
      project_dir_("")
    {
      ui_.setupUi(this);

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

      hierarchy_ = foundation::Memory::ConstructUnique<HierarchyView>(
        &foundation::Memory::default_allocator(),
        ui_.hierarchyView);

      ApplyStyle(app);

      connect(
        ui_.actionOpen_Project,
        &QAction::triggered,
        this,
        &MainWindow::OpenProject);

      ui_.gameWindow->installEventFilter(this);

      LoadLayout();
    }

    //--------------------------------------------------------------------------
    void MainWindow::ApplyStyle(QApplication* app)
    {
      app->setStyle(QStyleFactory::create("Fusion"));

      QPalette steam;
      steam.setColor(QPalette::Window, QColor(88, 106, 80));
      steam.setColor(QPalette::WindowText, Qt::white);
      steam.setColor(QPalette::Base, QColor(64, 70, 60));
      steam.setColor(QPalette::AlternateBase, QColor(88, 106, 80));
      steam.setColor(QPalette::ToolTipBase, Qt::white);
      steam.setColor(QPalette::ToolTipText, Qt::white);
      steam.setColor(QPalette::Text, Qt::white);
      steam.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
      steam.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
      steam.setColor(QPalette::Disabled, QPalette::Light, QColor(64, 70, 60));
      steam.setColor(QPalette::Button, QColor(88, 106, 80));
      steam.setColor(QPalette::ButtonText, Qt::white);
      steam.setColor(QPalette::BrightText, Qt::red);
      steam.setColor(QPalette::Link, QColor(159, 164, 98));

      steam.setColor(QPalette::Highlight, QColor(159, 164, 98));
      steam.setColor(QPalette::HighlightedText, Qt::black);

      app->setPalette(steam);

      QString dock_col = "rgb(111, 134, 102)";

      app->setStyleSheet("                      \
        QMainWindow::separator                  \
        {                                       \
          background-color: " + dock_col + ";   \
        }                                       \
        QDockWidget::title                      \
        {                                       \
          background-color: " + dock_col + ";   \
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
    void MainWindow::UpdateHierarchy(engine::Scene* scene)
    {
      ui_.hierarchyView->clear();

      const foundation::Vector<engine::TransformComponent*>& hierarchy =
        scene->hierarchy();

      for (size_t i = 0; i < hierarchy.size(); ++i)
      {
        AddSceneChild(hierarchy.at(i));
      }
    }

    //--------------------------------------------------------------------------
    void MainWindow::AddSceneChild(
      engine::TransformComponent* child,
      QTreeWidgetItem* item)
    {
      const engine::Entity* ent = child->entity();
      const foundation::String& name = ent->name();

      QTreeWidgetItem* new_item = new QTreeWidgetItem();
      new_item->setText(0, name.c_str());

      if (item == nullptr)
      {
        ui_.hierarchyView->addTopLevelItem(new_item);
      }
      else
      {
        item->addChild(new_item);
      }

      const foundation::Vector<engine::TransformComponent*>& children =
        child->children();

      for (size_t i = 0; i < children.size(); ++i)
      {
        AddSceneChild(children.at(i), new_item);
      }
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
  }
}
