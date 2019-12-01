#pragma once

#include <graphics/definitions/graphics_window.h>

#include <QWidget>

#include <functional>

namespace snuffbox
{
  namespace editor
  {
    class PlaybackControls;

    /**
    * @brief Used to show the rendering of Snuffbox within the editor
    *
    * @author Daniel Konings
    */
    class GameView : public QWidget
    {

      Q_OBJECT;

    public:
      
      /**
      * @brief The resize callback definition
      */
      using OnResize = std::function<void(uint16_t, uint16_t)>;

      /**
      * @brief Construct through a parent tree
      *
      * @param[in] parent The parent of this widget
      */
      GameView(QWidget* parent = nullptr);

      /**
      * @brief Creates a graphics window from the current Qt widget, to use
      *        with the renderer
      *
      * @remarks This uses the QWidget::winId as a window handle
      *
      * @return The created graphics window
      */
      graphics::GraphicsWindow GetGraphicsWindow() const;

      /**
      * @brief Bind a resize callback for when the game view is resized
      *
      * @param[in] on_resize The callback to bind
      */
      void BindResizeCallback(const OnResize& on_resize);

      /**
      * @return Is the window currently being resized?
      */
      bool IsResizing() const;

      /**
      * @brief Sets if playback should be enabled
      *
      * @param[in] enabled The new value
      */
      void SetPlaybackEnabled(bool enabled);

    protected:

      /**
      * @brief Disable painting
      *
      * @param[in] evt The paint event to discard
      */
      void paintEvent(QPaintEvent* evt) override;

      /**
      * @brief Used to send new size data to the engine when the game window
      *        is resized
      *
      * @param[in] evt The resize event
      */
      void resizeEvent(QResizeEvent* evt) override;

      /**
      * @brief Called when the resizing timer ends, updating the actual size
      *        of the renderer
      *
      * @param[in] evt The timer event
      */
      void timerEvent(QTimerEvent* evt) override;

    private:

      OnResize on_resize_; //!< The on resize callback
      QWidget* graphics_view_; //!< The window that we will render to
      PlaybackControls* playback_; //!< The playback controls

      QSize desired_size_; //!< The desired size of the rendering frame

      int size_timer_; //!< The time between resizing events

      static const int kResizeDelay_; //!< The delay inbetween resizes in ms
    };
  }
}