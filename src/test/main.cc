#include <engine/application/application.h>

using namespace snuffbox;
using namespace foundation;
using namespace engine;

int main(int argc, char** argv)
{
  Application::Configuration cfg;
  cfg.application_name = "snuffbox-hydra-test";
  cfg.version_string = "0.0";
  cfg.verbosity = 2;
  cfg.window_width = 1280;
  cfg.window_height = 720;
  cfg.editor_mode = false;

  Application app = Application(argc, argv, cfg);
  return static_cast<int>(app.Run());
}
