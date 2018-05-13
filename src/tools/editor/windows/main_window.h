#pragma once

#include "ui_main_window.h"
#include "tools/editor/windows/console.h"

#include <graphics/definitions/graphics_window.h>
#include <foundation/memory/memory.h>

#include <qmainwindow.h>

namespace snuffbox
{
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

      QString project_dir_; //!< The current project directory

      /**
      * @see GraphicsWindow::SizeCallback
      */
      graphics::GraphicsWindow::SizeCallback on_resize_;
    };
  }
}
