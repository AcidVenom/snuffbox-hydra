#pragma once

#include "tools/editor/windows/main_window.h"

#include <engine/application/application.h>
#include <tools/builder/builder.h>

#include <qapplication.h>

namespace snuffbox
{
  namespace editor
  {
    /**
    * @brief The editor application that runs the engine in editor mode
    *
    * This class derives from QApplication to be useful with all Qt widgets
    * and windows as well. By deriving from Application, we can provide
    * custom functionality while in editor mode or when in "play" mode.
    *
    * @see Application
    *
    * @author Daniel Konings
    */
    class EditorApplication : public engine::Application, public QApplication
    {

    public:

      /**
      * @see Application::Application
      *
      * @remarks Apparently QApplication needs the argc argument
      *          to be a reference, thus it is a reference in this
      *          constructor
      */
      EditorApplication(
        int& argc,
        char** argv, 
        const engine::Application::Configuration& cfg);

      /**
      * @see Application::Run
      */
      foundation::ErrorCodes Run() override;

      /**
      * @brief Sets the project directory and starts the builder on that
      *        directory
      *
      * @param[in] path The path to the existing source directory
      *
      * @return Was the source directory a valid directory?
      */
      bool SetProjectDirectory(const foundation::Path& path);

    protected:

      /**
      * @see Application::CreateRenderer
      */
      void CreateRenderer(engine::WindowService* window) override;

    private:

      builder::Builder builder_; //!< The builder service
      foundation::UniquePtr<MainWindow> window_; //!< The main window
    };
  }
}
