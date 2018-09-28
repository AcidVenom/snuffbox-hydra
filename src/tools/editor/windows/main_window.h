#pragma once

#include <graphics/definitions/graphics_window.h>

#include <QMainWindow>

class QDockWidget;

namespace snuffbox
{
  namespace editor
  {

    class GameView;

    /**
    * @brief The main window that contains all main editor functionalities
    *
    * @author Daniel Konings
    */
    class MainWindow : public QMainWindow
    {

      Q_OBJECT;

    public:

      /**
      * @brief Construct the main window in a parent tree
      *
      * @param[in] parent The parent of the main window
      */
      MainWindow(QWidget* parent = nullptr);

      /**
      * @see GameView::GetGraphicsWindow
      */
      graphics::GraphicsWindow GetGraphicsWindow() const;

    private:

      QDockWidget* game_view_widget_; //!< The central game view dock widget
      GameView* game_view_; //!< The game view

      static const int kMinWidth_; //!< The minimum width of the window
      static const int kMinHeight_; //!< The minumum height of the window
    };
  }
}