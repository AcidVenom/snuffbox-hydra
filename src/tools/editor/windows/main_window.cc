#include "tools/editor/windows/main_window.h"
#include "tools/editor/windows/console.h"

#include <qstylefactory.h>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    MainWindow::MainWindow(QApplication* app) :
      app_(app),
      console_(nullptr)
    {
      ui_.setupUi(this);

      QTextBrowser* output_windows[] =
      {
        ui_.allOutput,
        ui_.engineOutput,
        ui_.editorOutput,
        ui_.playerOutput,
        ui_.scriptOutput,
        ui_.builderOutput
      };

      console_ = foundation::Memory::ConstructUnique<Console>(
        &foundation::Memory::default_allocator(),
        ui_.outputTabs,
        output_windows);

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
      steamPalette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
      steamPalette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
      steamPalette.setColor(QPalette::Disabled, QPalette::Light, QColor(64, 70, 60));
      steamPalette.setColor(QPalette::Button, QColor(88, 106, 80));
      steamPalette.setColor(QPalette::ButtonText, Qt::white);
      steamPalette.setColor(QPalette::BrightText, Qt::red);
      steamPalette.setColor(QPalette::Link, QColor(159, 164, 98));

      steamPalette.setColor(QPalette::Highlight, QColor(159, 164, 98));
      steamPalette.setColor(QPalette::HighlightedText, Qt::black);

      app->setPalette(steamPalette);

      QString dock_col = "rgb(111, 134, 102)";

      app->setStyleSheet("                      \
        QMainWindow::separator                  \
        {                                       \
          background-color: " + dock_col + ";   \
        }                                       \
        QDockWidget::title                      \
        {                                       \
          background-color: " + dock_col + ";   \
          border: 1px solid rgb(88, 106, 80);   \
        }                                       \
        QDockWidget                             \
        {                                       \
          color: white;                         \
        }"
      );
    }
  }
}