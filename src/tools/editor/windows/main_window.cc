#include "tools/editor/windows/main_window.h"
#include "tools/editor/application/editor_application.h"

#include "tools/editor/editor-widgets/game_view.h"
#include "tools/editor/editor-widgets/console_widget.h"
#include "tools/editor/asset-browser/asset_browser.h"

#include "tools/editor/asset-browser/asset_icon.h"

#include <QDockWidget>
#include <QVBoxLayout>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    const int MainWindow::kMinWidth_ = 640;
    const int MainWindow::kMinHeight_ = 480;

    const QString MainWindow::kSettingsGeometryKey_ = "MainWindow.Geometry";
    const QString MainWindow::kSettingsStateKey_ = "MainWindow.State";

    //--------------------------------------------------------------------------
    MainWindow::MainWindow(EditorApplication* app, QWidget* parent) :
      QMainWindow(parent),
      app_(app),
      game_view_(nullptr)
    {
      setObjectName(QStringLiteral("MainWindow"));
      setMinimumSize(kMinWidth_, kMinHeight_);

      QDockWidget* game_view_widget = new QDockWidget(this);
      game_view_ = new GameView(game_view_widget);
      game_view_widget->setWidget(game_view_);
      game_view_widget->setWindowTitle(QStringLiteral("Game view"));
      game_view_widget->setObjectName(QStringLiteral("MainWindowGameDock"));
      game_view_widget->setMinimumSize(kMinWidth_ * 0.5, kMinHeight_ * 0.5);
      
      addDockWidget(Qt::DockWidgetArea::TopDockWidgetArea, game_view_widget);

      QDockWidget* assets_widget = new QDockWidget(this);
      AssetBrowser* asset_browser = new AssetBrowser(assets_widget);
      assets_widget->setWidget(asset_browser);
      assets_widget->setWindowTitle(QStringLiteral("Assets"));
      assets_widget->setObjectName(QStringLiteral("MainWindowAssetsDock"));

      Project& current_project = app->project();
      asset_browser->Refresh(
        current_project.GetCurrentBuildPath() + '/' + Project::kAssetDirectory);

      addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea, assets_widget);

      QDockWidget* console_widget = new QDockWidget(this);
      ConsoleWidget* console = new ConsoleWidget(console_widget);
      console_widget->setWidget(console);
      console_widget->setWindowTitle(QStringLiteral("Console"));
      console_widget->setObjectName(QStringLiteral("MainWindowConsoleDock"));

      foundation::Logger::RedirectOutput(&ConsoleWidget::OnLog, console);

      addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea, console_widget);

      LoadWindowGeometry();
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
    }

    //--------------------------------------------------------------------------
    void MainWindow::LoadWindowGeometry()
    {
      QSettings& settings = EditorApplication::Instance()->GlobalSettings();

      if (
        settings.contains(kSettingsGeometryKey_) == true &&
        settings.contains(kSettingsStateKey_) == true)
      {
        restoreGeometry(settings.value(kSettingsGeometryKey_).toByteArray());
        restoreState(settings.value(kSettingsStateKey_).toByteArray());
      }
    }

    //--------------------------------------------------------------------------
    void MainWindow::closeEvent(QCloseEvent* evt)
    {
      Q_UNUSED(evt);

      SaveWindowGeometry();
    }
  }
}