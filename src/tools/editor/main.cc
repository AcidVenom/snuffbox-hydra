#include "tools/editor/windows/main_window.h"
#include "tools/editor/application/editor_application.h"

#include <foundation/memory/memory.h>

using namespace snuffbox;
using namespace foundation;
using namespace editor;

int main(int argc, char** argv)
{
  EditorApplication::Configuration cfg;
  cfg.application_name = "snuffbox-editor";
  cfg.version_string = "0.0";
  cfg.editor_mode = true;
  cfg.verbosity = 1;
  cfg.window_height = 0;
  cfg.window_width = 0;

  EditorApplication app(argc, argv, cfg);

  UniquePtr<MainWindow> window = 
    Memory::ConstructUnique<MainWindow>(&Memory::default_allocator(), &app);

  window->show();

  return static_cast<int>(app.Run());
}