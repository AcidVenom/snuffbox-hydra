#include "tools/editor/definitions/editor_colors.h"

#include <qpalette.h>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    const QPalette& EditorColors::DefaultPalette()
    {
      static bool initialized = false;
      static QPalette steam;

      if (initialized == false)
      {
        steam.setColor(QPalette::Window, QColor(88, 106, 80));
        steam.setColor(QPalette::WindowText, Qt::white);
        steam.setColor(QPalette::Base, QColor(64, 70, 60));
        steam.setColor(QPalette::AlternateBase, QColor(88, 106, 80));
        steam.setColor(QPalette::ToolTipBase, Qt::white);
        steam.setColor(QPalette::ToolTipText, Qt::white);
        steam.setColor(QPalette::Text, Qt::white);
        steam.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
        steam.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
        steam.setColor(QPalette::Disabled, QPalette::Light, QColor(64, 70, 60));
        steam.setColor(QPalette::Button, QColor(88, 106, 80));
        steam.setColor(QPalette::ButtonText, Qt::white);
        steam.setColor(QPalette::BrightText, Qt::red);
        steam.setColor(QPalette::Link, QColor(159, 164, 98));

        steam.setColor(QPalette::Highlight, QColor(159, 164, 98));
        steam.setColor(QPalette::HighlightedText, Qt::black);

        initialized = true;
      }

      return steam;
    }

    //--------------------------------------------------------------------------
    const QColor& EditorColors::DockColor()
    {
      static QColor color(111, 134, 102);
      return color;
    }

    //--------------------------------------------------------------------------
    ColorFormat EditorColors::ConsoleColor(foundation::LogSeverity sev)
    {
      ColorFormat color;
      color.foreground = QColor(255, 255, 255, 255);
      color.background = QColor(0, 0, 0, 0);

      switch (sev)
      {

      case foundation::LogSeverity::kDebug:
        color.foreground = QColor(128, 128, 128);
        color.background = QColor(0, 0, 0, 0);
        break;

      case foundation::LogSeverity::kInfo:
        color.foreground = QColor(255, 255, 255);
        color.background = QColor(30, 30, 30, 30);
        break;

      case foundation::LogSeverity::kWarning:
        color.foreground = QColor(255, 185, 0);
        color.background = QColor(70, 65, 15);
        break;

      case foundation::LogSeverity::kSuccess:
        color.foreground = QColor(35, 255, 0);
        color.background = QColor(70, 120, 55);
        break;

      case foundation::LogSeverity::kError:
        color.foreground = QColor(255, 0, 0);
        color.background = QColor(60, 0, 0);
        break;

      case foundation::LogSeverity::kFatal:
        color.foreground = QColor(255, 255, 255);
        color.background = QColor(255, 0, 0);
        break;

      }

      return color;
    }
  }
}