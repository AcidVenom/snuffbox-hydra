#include "tools/editor/windows/main_window.h"

#include <qstylefactory.h>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    MainWindow::MainWindow(QApplication* app) :
      app_(app)
    {
      ui_.setupUi(this);
      ApplyStyle(app);
    }

    //--------------------------------------------------------------------------
    void MainWindow::ApplyStyle(QApplication* app)
    {
      app->setStyle(QStyleFactory::create("Fusion"));

      QPalette steamPalette;
      steamPalette.setColor(QPalette::Window, QColor(88, 106, 80));
      steamPalette.setColor(QPalette::WindowText, Qt::white);
      steamPalette.setColor(QPalette::Base, QColor(64, 70, 60));
      steamPalette.setColor(QPalette::AlternateBase, QColor(88, 106, 80));
      steamPalette.setColor(QPalette::ToolTipBase, Qt::white);
      steamPalette.setColor(QPalette::ToolTipText, Qt::white);
      steamPalette.setColor(QPalette::Text, Qt::white);
      steamPalette.setColor(QPalette::Button, QColor(88, 106, 80));
      steamPalette.setColor(QPalette::ButtonText, Qt::white);
      steamPalette.setColor(QPalette::BrightText, Qt::red);
      steamPalette.setColor(QPalette::Link, QColor(159, 164, 98));

      steamPalette.setColor(QPalette::Highlight, QColor(159, 164, 98));
      steamPalette.setColor(QPalette::HighlightedText, Qt::black);

      app->setPalette(steamPalette);
    }
  }
}