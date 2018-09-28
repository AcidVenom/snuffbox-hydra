#include "tools/editor/windows/main_window.h"
#include "tools/editor/windows/game_view.h"

#include <QDockWidget>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    const int MainWindow::kMinWidth_ = 640;
    const int MainWindow::kMinHeight_ = 480;

    //--------------------------------------------------------------------------
    MainWindow::MainWindow(QWidget* parent) :
      QMainWindow(parent),
      game_view_widget_(nullptr),
      game_view_(nullptr)
    {
      setObjectName(QStringLiteral("MainWindow"));
      setMinimumSize(kMinWidth_, kMinHeight_);

      game_view_widget_ = new QDockWidget(this);
      game_view_ = new GameView(game_view_widget_);
      game_view_widget_->setWidget(game_view_);
      game_view_widget_->setObjectName(QStringLiteral("MainWindowGameDock"));
      game_view_widget_->setMinimumSize(kMinWidth_ * 0.5, kMinHeight_ * 0.5);
      
      addDockWidget(Qt::DockWidgetArea::TopDockWidgetArea, game_view_widget_);
    }

    //--------------------------------------------------------------------------
    graphics::GraphicsWindow MainWindow::GetGraphicsWindow() const
    {
      return game_view_->GetGraphicsWindow();
    }
  }
}