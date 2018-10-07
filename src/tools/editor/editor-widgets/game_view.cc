#include "tools/editor/editor-widgets/game_view.h"

#include <QWindow>
#include <QResizeEvent>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    GameView::GameView(QWidget* parent) :
      QWidget(parent),
      on_resize_(nullptr)
    {
      setMouseTracking(true);
      setAutoFillBackground(false);
    }

    //--------------------------------------------------------------------------
    graphics::GraphicsWindow GameView::GetGraphicsWindow() const
    {
      graphics::GraphicsWindow gw;
      gw.handle = reinterpret_cast<void*>(winId());
      gw.width = width();
      gw.height = height();

      return gw;
    }

    //--------------------------------------------------------------------------
    void GameView::paintEvent(QPaintEvent* evt)
    {
      Q_UNUSED(evt);
    }

    //--------------------------------------------------------------------------
    void GameView::resizeEvent(QResizeEvent* evt)
    {
      QSize new_size = evt->size();

      if (on_resize_ != nullptr)
      {
        on_resize_(
          static_cast<uint16_t>(new_size.width()), 
          static_cast<uint16_t>(new_size.height()));
      }
    }

    //--------------------------------------------------------------------------
    void GameView::BindResizeCallback(const OnResize& on_resize)
    {
      on_resize_ = on_resize;
    }
  }
}