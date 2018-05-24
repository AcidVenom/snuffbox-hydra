#pragma once

#include "ui_main_window.h"

#include "tools/editor/windows/console.h"
#include "tools/editor/windows/hierarchy_view.h"

#include <graphics/definitions/graphics_window.h>
#include <foundation/memory/memory.h>

#include <qmainwindow.h>

namespace snuffbox
{
  namespace engine
  {
    class Scene;
    class TransformComponent;
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
      * @brief Updates the scene inspector with the current transform
      *        hierarchy
      *
      * @see Scene::UpdateHierarchy
      *
      * @param[in] scene The scene to retrieve the hierarchy from
      */
      void UpdateHierarchy(engine::Scene* scene);

      /**
      * @brief Adds a transform component as a child item of the hierarchy
      *
      * @param[in] child The child to add
      * @param[in] item The parent item, if nullptr the item is added as a top
      *                 level item
      */
      void AddSceneChild(
        engine::TransformComponent* child,
        QTreeWidgetItem* item = nullptr);

    private slots:

      /**
      * @brief Opens a folder browser dialog to open a project directory
      *        and sends it to the EditorApplication
      *
      * @see EditorApplication::SetProjectDirectory
      */
      void OpenProject();

    private:

      Ui::MainWindow ui_; //!< The user interface definition made in Designer
      EditorApplication* app_; //!< A reference to the main editor application

      foundation::UniquePtr<Console> console_; //!< The console
      foundation::UniquePtr<HierarchyView> hierarchy_; //!< The hierarchy view

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
