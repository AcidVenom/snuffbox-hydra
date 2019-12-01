#include "tools/editor/editor-widgets/game_view.h"

#include "tools/editor/editor-widgets/playback_controls.h"

#include <QWindow>
#include <QResizeEvent>
#include <QTimer>
#include <QVBoxLayout>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    const int GameView::kResizeDelay_ = 100;

    //--------------------------------------------------------------------------
    GameView::GameView(QWidget* parent) :
      QWidget(parent),
      on_resize_(nullptr),
      graphics_view_(nullptr),
      playback_(nullptr),
      desired_size_(QSize(0, 0)),
      size_timer_(-1)
    {
      setObjectName(QStringLiteral("GameViewWidget"));

      QVBoxLayout* main_layout = new QVBoxLayout(this);
      main_layout->setObjectName(QStringLiteral("GameViewMainLayout"));
      main_layout->setContentsMargins(0, 0, 0, 0);

      playback_ = new PlaybackControls(this);
      main_layout->addWidget(playback_);

      graphics_view_ = new QWidget(this);
      graphics_view_->setObjectName(QStringLiteral("GameGraphicsView"));
      main_layout->addWidget(graphics_view_);

      setMouseTracking(true);
      setAutoFillBackground(false);

      setFocusPolicy(Qt::FocusPolicy::StrongFocus);

      setLayout(main_layout);
    }

    //--------------------------------------------------------------------------
    graphics::GraphicsWindow GameView::GetGraphicsWindow() const
    {
      graphics::GraphicsWindow gw;
      gw.handle = reinterpret_cast<void*>(graphics_view_->winId());
      gw.width = graphics_view_->width();
      gw.height = graphics_view_->height();

      return gw;
    }

    //--------------------------------------------------------------------------
    void GameView::BindResizeCallback(const OnResize& on_resize)
    {
      on_resize_ = on_resize;
    }

    //--------------------------------------------------------------------------
    bool GameView::IsResizing() const
    {
      return size_timer_ >= 0;
    }

    //--------------------------------------------------------------------------
    void GameView::SetPlaybackEnabled(bool enabled)
    {
      playback_->DisableAll(enabled == false);
    }

    //--------------------------------------------------------------------------
    void GameView::paintEvent(QPaintEvent* evt)
    {
      Q_UNUSED(evt);
    }

    //--------------------------------------------------------------------------
    void GameView::resizeEvent(QResizeEvent* evt)
    {
      desired_size_ = graphics_view_->size();
      
      if (size_timer_ >= 0)
      {
        killTimer(size_timer_);
        size_timer_ = -1;
      }

      size_timer_ = startTimer(kResizeDelay_);
    }

    //--------------------------------------------------------------------------
    void GameView::timerEvent(QTimerEvent* evt)
    {
      if (evt->timerId() == size_timer_)
      {
        on_resize_(
          static_cast<uint16_t>(desired_size_.width()),
          static_cast<uint16_t>(desired_size_.height()));

        killTimer(size_timer_);
        size_timer_ = -1;
      }
    }
  }
}