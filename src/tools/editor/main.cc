#include "tools/editor/application/editor_application.h"

using namespace snuffbox;
using namespace foundation;
using namespace editor;

int main(int argc, char** argv)
{
  EditorApplication::Configuration cfg;
  cfg.application_name = "snuffbox-editor";
  cfg.version_string = "0.0";
  cfg.editor_mode = true;
  cfg.verbosity = 2;
  cfg.window_height = 0;
  cfg.window_width = 0;

  EditorApplication app(argc, argv, cfg);

  return static_cast<int>(app.Run());
}
