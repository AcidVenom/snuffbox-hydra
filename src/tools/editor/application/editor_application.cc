#include "tools/editor/application/editor_application.h"

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    EditorApplication::EditorApplication(
      int argc,
      char** argv,
      const engine::Application::Configuration& cfg)
      :
      Application(argc, argv, cfg),
      QApplication(argc, argv)
    {
      
    }

    //--------------------------------------------------------------------------
    foundation::ErrorCodes EditorApplication::Run()
    {
      ApplyConfiguration();

      foundation::Logger::LogVerbosity<2>(
        foundation::LogChannel::kEditor,
        foundation::LogSeverity::kDebug,
        "Initializing the editor"
        );

      foundation::ErrorCodes err = Initialize();

      if (err != foundation::ErrorCodes::kSuccess)
      {
        Shutdown();
        return err;
      }

      while (should_quit() == false)
      {
        processEvents();
      }

      Shutdown();

      return foundation::ErrorCodes::kSuccess;
    }
  }
}