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
      * @return The game view
      */
      GameView* game_view() const;

    protected:

      /**
      * @brief Saves the current window geometry, to maintain sizing between
      *        different sessions
      */
      void SaveWindowGeometry();

      /**
      * @brief Load an old window geometry if available
      */
      void LoadWindowGeometry();

      /**
      * @brief Save the current window geometry
      */
      void closeEvent(QCloseEvent* evt) override;

    private:

      QDockWidget* game_view_widget_; //!< The central game view dock widget
      QDockWidget* console_widget_; //!< The console widget

      GameView* game_view_; //!< The game view

      static const int kMinWidth_; //!< The minimum width of the window
      static const int kMinHeight_; //!< The minumum height of the window

      static const QString kSettingsGeometryKey_; //!< The geometry settings key
      static const QString kSettingsStateKey_; //!< The state settings key
    };
  }
}