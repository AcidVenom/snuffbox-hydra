#include "tools/editor/application/styling.h"

#include <QApplication>
#include <QPalette>
#include <QToolTip>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    QColor Styling::kColorRoles_[Styling::ColorRole::kCount];

    //--------------------------------------------------------------------------
    void Styling::ApplyStyle(QApplication* app)
    {
      app->setStyle("fusion");

      using R = QPalette::ColorRole;
      using G = QPalette::ColorGroup;
      using CR = ColorRole;

      QPalette p;
      p.setColor(R::Background, GetStyleColor(CR::kBackground));
      p.setColor(R::Base, GetStyleColor(CR::kBase));
      p.setColor(R::Text, GetStyleColor(CR::kText));
      p.setColor(R::HighlightedText, GetStyleColor(CR::kHighlightedText));
      p.setColor(R::ButtonText, GetStyleColor(CR::kButtonText));
      p.setColor(R::WindowText, GetStyleColor(CR::kWindowText));
      p.setColor(R::BrightText, GetStyleColor(CR::kBrightText));
      p.setColor(G::Disabled, R::Text, GetStyleColor(CR::kDisabledText));
      p.setColor(G::Disabled, R::Light, GetStyleColor(CR::kDisabledLight));
      p.setColor(R::Button, GetStyleColor(CR::kButton));
      p.setColor(R::Highlight, GetStyleColor(CR::kHighlight));
      p.setColor(R::ToolTipBase, GetStyleColor(CR::kTooltipBase));
      p.setColor(R::ToolTipText, GetStyleColor(CR::kTooltipText));

      QToolTip::setPalette(p);
      app->setPalette(p);
    }

    //--------------------------------------------------------------------------
    QColor Styling::GetStyleColor(ColorRole role, int a)
    {
      static bool initialized_roles = false;
      if (initialized_roles == false)
      {
        kColorRoles_[ColorRole::kBackground] = QColor(50, 50, 60);
        kColorRoles_[ColorRole::kBase] = QColor(30, 30, 40);
        kColorRoles_[ColorRole::kText] = QColor(Qt::white);
        kColorRoles_[ColorRole::kHighlightedText] = QColor(Qt::black);
        kColorRoles_[ColorRole::kButtonText] = QColor(Qt::white);
        kColorRoles_[ColorRole::kWindowText] = QColor(Qt::white);
        kColorRoles_[ColorRole::kBrightText] = QColor(Qt::white);
        kColorRoles_[ColorRole::kDisabledText] = QColor(Qt::darkGray);
        kColorRoles_[ColorRole::kDisabledLight] = QColor(Qt::black);
        kColorRoles_[ColorRole::kButton] = QColor(55, 70, 80);
        kColorRoles_[ColorRole::kHighlight] = QColor(50, 200, 0);
        kColorRoles_[ColorRole::kTooltipBase] = QColor(200, 255, 100);
        kColorRoles_[ColorRole::kTooltipText] = QColor(Qt::black);
        kColorRoles_[ColorRole::kSelected] = QColor(40, 120, 60);
        kColorRoles_[ColorRole::kClickable] = QColor(40, 70, 70);
        kColorRoles_[ColorRole::kXAxis] = QColor(200, 30, 30);
        kColorRoles_[ColorRole::kYAxis] = QColor(0, 125, 0);
        kColorRoles_[ColorRole::kZAxis] = QColor(0, 125, 255);
        kColorRoles_[ColorRole::kWAxis] = QColor(Qt::black);
        kColorRoles_[ColorRole::kBlueButton] = QColor(10, 70, 130);
        kColorRoles_[ColorRole::kValid] = QColor(0, 85, 30);
        kColorRoles_[ColorRole::kInvalid] = QColor(125, 30, 30);

        initialized_roles = true;
      }

      QColor col = kColorRoles_[role];
      col.setAlpha(a);

      return col;
    }

    //--------------------------------------------------------------------------
    QString Styling::GetCSSColor(const QColor& color)
    {
      return QString("rgba(%0, %1, %2, %3)").arg(
        QString::number(color.red()), 
        QString::number(color.green()), 
        QString::number(color.blue()), 
        QString::number(static_cast<float>(color.alpha()) / 255.0f));
    }

    //--------------------------------------------------------------------------
    QString Styling::GetStyleColorCSS(ColorRole role, int a)
    {
      QColor color = GetStyleColor(role, a);
      return GetCSSColor(color);
    }

    //--------------------------------------------------------------------------
    QString Styling::GradientCSS(
      const QColor& a, 
      const QColor& b, 
      bool horizontal)
    {
      float x1 = 0.0f;
      float y1 = 0.0f;

      float x2 = horizontal == true ? 1.0f : 0.0f;
      float y2 = horizontal == true ? 0.0f : 1.0f;

      QString css_a = GetCSSColor(a);
      QString css_b = GetCSSColor(b);

      QString format = 
        "qlineargradient("
        "spread: pad,"
        "x1: %0,"
        "y1: %1,"
        "x2: %2,"
        "y2: %3,"
        "stop: 0 %4, stop: 1 %5);";

      return format.arg(
        QString::number(x1), 
        QString::number(y1), 
        QString::number(x2),
        QString::number(y2), 
        css_a, 
        css_b);
    }
  }
}