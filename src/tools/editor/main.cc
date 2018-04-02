#include "tools/editor/windows/main_window.h"

#include <foundation/memory/memory.h>

#include <QtWidgets/qapplication.h>

using namespace snuffbox;
using namespace foundation;
using namespace editor;

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  UniquePtr<MainWindow> window = 
    Memory::ConstructUnique<MainWindow>(&Memory::default_allocator(), &app);

  window->show();

  return app.exec();
}