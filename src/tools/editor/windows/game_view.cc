#include "tools/editor/windows/game_view.h"

#include <QWindow>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    GameView::GameView(QWidget* parent) :
      QWidget(parent)
    {
      setMouseTracking(true);
      setAutoFillBackground(false);

      setFixedSize(1280, 720);
    }

    //--------------------------------------------------------------------------
    graphics::GraphicsWindow GameView::GetGraphicsWindow() const
    {
      graphics::GraphicsWindow gw;
      gw.handle = reinterpret_cast<void*>(winId());
      gw.width = 1280;
      gw.height = 720;

      return gw;
    }

    //--------------------------------------------------------------------------
    void GameView::paintEvent(QPaintEvent* evt)
    {
      Q_UNUSED(evt);
    }
  }
}