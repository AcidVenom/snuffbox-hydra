#include "tools/editor/application/editor_application.h"

using namespace snuffbox;
using namespace foundation;
using namespace editor;

#include "tools/editor/property-editor/property_value.h"
#include <engine/ecs/entity.h>

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

  foundation::ErrorCodes err = foundation::ErrorCodes::kSuccess;
  while ((err = app.Run()) == foundation::ErrorCodes::kRestart);

  return static_cast<int>(err);
}
