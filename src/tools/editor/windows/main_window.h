#pragma once

#include <tools/compilers/definitions/asset_types.h>
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
    class PropertyView;

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
      * @brief Creates the menu bar at the top of the window
      */
      void CreateMenuBar();

      /**
      * @brief Refreshes the current asset list
      */
      void RefreshAssetList();

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
      * @brief Called when the 'File -> Open project' action is triggered
      */
      void OnOpenProject();

      /**
      * @brief Called when the 'File -> New scene' action is triggered
      */
      void OnNewScene();

      /**
      * @brief Called when the 'File -> Save scene' action is triggered
      */
      void OnSaveScene();

      /**
      * @brief Called when the 'File -> Save scene as' action is triggered
      */
      void OnSaveSceneAs();

      /**
      * @brief Shows a dialog whether or not to exit the application
      *
      * @return Was the dialog accepted?
      */
      bool ConfirmExit();

      /**
      * @brief Save the current window geometry
      */
      void closeEvent(QCloseEvent* evt) override;

    public slots:

      void OnAssetImported(
        compilers::AssetTypes type, 
        const QString& full_path);

    signals:

      /**
      * @brief Emitted when an undo has been triggered
      */
      void Undone();

      /**
      * @brief Emitted when a redo has been triggered
      */
      void Redone();

      /**
      * @brief Emitted when the current scene should be refreshed
      */
      void SceneRefresh();

    public:

      /**
      * @brief Called when a frame has finished
      */
      void OnUpdate();

      /**
      * @return Is the game view currently being resized?
      */
      bool IsResizing() const;

      /**
      * @brief Stop redirecting log output
      */
      ~MainWindow();

    private:

      EditorApplication* app_; //!< The editor application
      GameView* game_view_; //!< The game view
      AssetBrowser* asset_browser_; //!< The asset browser
      PropertyView* properties_; //!< The current property view

      static const int kMinWidth_; //!< The minimum width of the window
      static const int kMinHeight_; //!< The minumum height of the window

      static const QString kSettingsGeometryKey_; //!< The geometry settings key
      static const QString kSettingsStateKey_; //!< The state settings key
      static const QString kSettingsSizeKey_; //!< The size of the window key
    };
  }
}