#pragma once

#include <graphics/definitions/graphics_window.h>

#include <QWidget>

namespace snuffbox
{
  namespace editor
  {
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

    protected:

      /**
      * @brief Disable painting
      *
      * @param[in] evt The paint event to discard
      */
      void paintEvent(QPaintEvent* evt) override;
    };
  }
}