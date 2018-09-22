#include "tools/editor/application/styling.h"

#include <QApplication>
#include <QPalette>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    void Styling::ApplyStyle(QApplication* app)
    {
      app->setStyle("fusion");

      using Role = QPalette::ColorRole;
      using Group = QPalette::ColorGroup;

      QPalette palette;
      palette.setColor(Role::Background, QColor(50, 50, 50));
      palette.setColor(Role::Base, QColor(30, 30, 30));
      palette.setColor(Role::Text, QColor(Qt::white));
      palette.setColor(Role::HighlightedText, QColor(Qt::black));
      palette.setColor(Role::ButtonText, QColor(Qt::white));
      palette.setColor(Role::WindowText, QColor(Qt::white));
      palette.setColor(Role::BrightText, QColor(Qt::white));
      palette.setColor(Role::Button, QColor(65, 70, 65));
      palette.setColor(Role::Highlight, QColor(100, 200, 0));

      app->setPalette(palette);
    }
  }
}