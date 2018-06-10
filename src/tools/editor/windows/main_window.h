#pragma once

#include "ui_main_window.h"

#include "tools/editor/windows/console.h"
#include "tools/editor/windows/hierarchy_view.h"
#include "tools/editor/windows/inspector.h"
#include "tools/editor/windows/asset_browser.h"

#include <graphics/definitions/graphics_window.h>
#include <foundation/memory/memory.h>

#include <qmainwindow.h>

namespace snuffbox
{
  namespace engine
  {
    class SceneService;
  }

  namespace editor
  {
    class EditorApplication;

    /**
    * @brief The main window of the editor application
    *
    * The class is the main access point for everything related in the editor
    * and is the first element that is instantiated within the QApplication.
    * The UI itself is made in Qt5's Designer and is in the source as XML as
    * well. AUTOUIC in CMake makes sure this UI gets properly compiled every
    * time.
    *
    * @author Daniel Konings
    */
    class MainWindow : public QMainWindow
    {

      Q_OBJECT

      friend EditorApplication;

    public:

      /**
      * @brief Construct through the application
      *
      * Sets up the internal UI to this window
      *
      * @param[in] app The main editor application, which functions as a
      *                QApplication as well
      */
      MainWindow(EditorApplication* app);

      /**
      * @brief Applies the style I've used in "snuffbox", which are
      *        basically Steam's older colors.
      *
      * @see https://github.com/AcidVenom/snuffbox/blob/master/snuffbox/console/console.cc
      *
      * @param[in] app The main Qt application
      */
      void ApplyStyle(QApplication* app);

      /**
      * @brief Override close event so that it closes the main EditorApplication
      */
      void closeEvent(QCloseEvent* evt) override;

      /**
      * @brief Used to send resize events and input data from Qt to the engine
      */
      bool eventFilter(QObject* obj, QEvent* evt) override;

      /**
      * @return The graphics window data from the central player widget
      */
      graphics::GraphicsWindow GetGraphicsWindow() const;

      /**
      * @see GraphicsWindow::SizeCallback
      *
      * @brief Actually bind the callback here
      */
      void BindResizeCallback(const graphics::GraphicsWindow::SizeCallback& cb);

    protected:

      /**
      * @brief Binds all required events for interaction between interfaces
      */
      void BindEvents();

      /**
      * @brief Creates the console after initialization of the main window
      */
      void CreateConsole();

      /**
      * @brief Initializes the hierarchy inspector with the scene service
      *        to sync with
      *
      * @param[in] scene_service The current scene service to sync with
      */
      void CreateHierarchy(engine::SceneService* scene_service);

      /**
      * @brief Creates the inspector to inspect entities and their
      *        components with
      */
      void CreateInspector();

      /**
      * @brief Creates the asset browser to view the current build directory
      *        with
      */
      void CreateAssetBrowser();

      /**
      * @brief Loads the layout the user had during previous execution,
      *        using QSettings
      */
      void LoadLayout();

      /**
      * @brief Saves the layout the user has during current execution,
      *        storing them in QSettings
      */
      void SaveLayout();

      /**
      * @brief Marks a playback button that it is being used
      *
      * @remarks Changes the button's background color to 
      *          EditorColors::BlueButton
      *
      * @param[in] button The button to change
      * @param[in] enabled Should we enable the blue background or change
      *                    it back?
      */
      void MarkPlaybackButton(QPushButton* button, bool enabled);

      /**
      * @brief Sets whether we're able to start the playback or not by
      *        disabling the play button
      *
      * @param[in] enabled Is playback currently enabled?
      */
      void SetPlaybackEnabled(bool enabled);

    private slots:

      /**
      * @brief Opens a folder browser dialog to open a project directory
      *        and sends it to the EditorApplication
      *
      * @see EditorApplication::SetProjectDirectory
      */
      void OpenProject();

      /**
      * @brief Called when an entity was selected in the hierarchy
      *
      * @param[in] entity The entity that was selected
      */
      void OnSelectEntity(engine::Entity* entity);

      /**
      * @brief Called when the play button was pressed, changes the
      *        EditorApplication state to EditorApplication::States::kPlaying
      */
      void OnPlay();

      /**
      * @brief Called when an asset in the asset browser has been double
      *        clicked
      */
      void OnDoubleClickedAsset(QString relative, int type);

    signals:

      /**
      * @brief A signal to emit when the scene changed
      */
      void SceneChanged();

    public:

      /**
      * @return A reference to the main editor application
      */
      EditorApplication* app() const;

    private:

      Ui::MainWindow ui_; //!< The user interface definition made in Designer
      EditorApplication* app_; //!< A reference to the main editor application

      foundation::UniquePtr<Console> console_; //!< The console
      foundation::UniquePtr<HierarchyView> hierarchy_; //!< The hierarchy view
      foundation::UniquePtr<Inspector> inspector_; //!< The inspector
      foundation::UniquePtr<AssetBrowser> asset_browser_; //!< The asset browser

      QString project_dir_; //!< The current project directory

      /**
      * @see GraphicsWindow::SizeCallback
      */
      graphics::GraphicsWindow::SizeCallback on_resize_;

      static const char* kCompanyName_; //!< The company name for QSettings
      static const char* kAppName_; //!< The app name for QSettings

      /**
      * @brief The name to save the window geometry at in the QSettings
      */
      static const char* kSaveGeometry_;

      /**
      * @brief The name to save the window state at in the QSettings
      */
      static const char* kSaveWindow_;

      /**
      * @brief The name to save the console splitter state at in the QSettings
      */
      static const char* kSaveSplitterA_;

      /**
      * @brief The name to save the file tree splitter state at in the QSettings
      */
      static const char* kSaveSplitterB_;
    };
  }
}
