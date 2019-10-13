#include "tools/editor/application/styling.h"

#include <QApplication>
#include <QPalette>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    const char* Styling::kColorRoles_[Styling::ColorRole::kCount];

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
      palette.setColor(Group::Disabled, Role::Text, QColor(Qt::darkGray));
      palette.setColor(Group::Disabled, Role::Light, QColor(Qt::black));
      palette.setColor(Role::Button, QColor(65, 70, 65));
      palette.setColor(Role::Highlight, QColor(100, 200, 0));

      app->setPalette(palette);
    }

    //--------------------------------------------------------------------------
    const char* Styling::GetStyleColor(ColorRole role)
    {
      static bool initialized_roles = false;
      if (initialized_roles == false)
      {
        kColorRoles_[ColorRole::kSelected] = "rgb(50, 100, 0)";
        kColorRoles_[ColorRole::kClickable] = "rgb(61, 68, 67)";
        kColorRoles_[ColorRole::kHighlight] = "rgb(100, 200, 0)";
        initialized_roles = true;
      }

      return kColorRoles_[role];
    }
  }
}