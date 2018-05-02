#pragma once

#include <engine/application/application.h>
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
      */
      EditorApplication(
        int argc, 
        char** argv, 
        const engine::Application::Configuration& cfg);

      /**
      * @see Application::Run
      */
      foundation::ErrorCodes Run() override;
    };
  }
}