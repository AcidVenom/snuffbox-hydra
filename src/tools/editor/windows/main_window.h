#pragma once

#include <graphics/definitions/graphics_window.h>

#include <QMainWindow>

class QDockWidget;

namespace snuffbox
{
  namespace editor
  {
    class EditorApplication;
    class AssetBrowser;
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
      * @param[in] app The editor application to retrieve settings from
      *                and to link signals to
      * @param[in] parent The parent of the main window
      */
      MainWindow(EditorApplication* app, QWidget* parent = nullptr);

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
      * @brief Window size is saved seperately, so we can set the window size
      *        before adding any new objects, to make sure they all fit in
      */
      void LoadWindowSize();

      /**
      * @brief Save the current window geometry
      */
      void closeEvent(QCloseEvent* evt) override;

    private:

      EditorApplication* app_; //!< The editor application
      GameView* game_view_; //!< The game view
      AssetBrowser* asset_browser_; //!< The asset browser

      static const int kMinWidth_; //!< The minimum width of the window
      static const int kMinHeight_; //!< The minumum height of the window

      static const QString kSettingsGeometryKey_; //!< The geometry settings key
      static const QString kSettingsStateKey_; //!< The state settings key
      static const QString kSettingsSizeKey_; //!< The size of the window key
    };
  }
}