#include "tools/editor/application/styling.h"

#include <QApplication>
#include <QPalette>
#include <QToolTip>

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
      palette.setColor(Role::ToolTipBase, QColor(200, 255, 100));
      palette.setColor(Role::ToolTipText, QColor(Qt::black));

      QToolTip::setPalette(palette);

      app->setPalette(palette);
    }

    //--------------------------------------------------------------------------
    QString Styling::GetStyleColor(ColorRole role, float a)
    {
      static bool initialized_roles = false;
      if (initialized_roles == false)
      {
        kColorRoles_[ColorRole::kSelected] = "rgba(50, 100, 0, %0)";
        kColorRoles_[ColorRole::kClickable] = "rgba(61, 75, 67, %0)";
        kColorRoles_[ColorRole::kHighlight] = "rgba(100, 200, 0, %0)";
        initialized_roles = true;
      }

      QString format = kColorRoles_[role];
      return format.arg(std::to_string(a).c_str());
    }
  }
}