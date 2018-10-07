#include "tools/editor/windows/main_window.h"
#include "tools/editor/application/editor_application.h"
#include "tools/editor/editor-widgets/game_view.h"
#include "tools/editor/editor-widgets/console_widget.h"

#include <QDockWidget>

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
    MainWindow::MainWindow(QWidget* parent) :
      QMainWindow(parent),
      game_view_widget_(nullptr),
      console_widget_(nullptr),
      game_view_(nullptr)
    {
      setObjectName(QStringLiteral("MainWindow"));
      setMinimumSize(kMinWidth_, kMinHeight_);

      game_view_widget_ = new QDockWidget(this);
      game_view_ = new GameView(game_view_widget_);
      game_view_widget_->setWidget(game_view_);
      game_view_widget_->setWindowTitle(QStringLiteral("Game view"));
      game_view_widget_->setObjectName(QStringLiteral("MainWindowGameDock"));
      game_view_widget_->setMinimumSize(kMinWidth_ * 0.5, kMinHeight_ * 0.5);
      
      addDockWidget(Qt::DockWidgetArea::TopDockWidgetArea, game_view_widget_);

      console_widget_ = new QDockWidget(this);
      ConsoleWidget* console = new ConsoleWidget(console_widget_);
      console_widget_->setWidget(console);
      console_widget_->setWindowTitle(QStringLiteral("Console"));
      console_widget_->setObjectName(QStringLiteral("MainWindowConsoleDock"));

      foundation::Logger::RedirectOutput(&ConsoleWidget::OnLog, console);

      addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea, console_widget_);

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