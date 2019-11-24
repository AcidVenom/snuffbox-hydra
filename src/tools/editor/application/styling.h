#pragma once

class QApplication;

#include <QString>
#include <QColor>

namespace snuffbox
{
  namespace editor
  {
    /**
    * @brief Used to set styling within the QApplication
    *
    * @author Daniel Konings
    */
    class Styling
    {

    public:

      /**
      * The different colors that exist to use globally
      */
      enum ColorRole : int
      {
        kBackground,
        kBase,
        kText,
        kHighlightedText,
        kButtonText,
        kWindowText,
        kBrightText,
        kDisabledText,
        kDisabledLight,
        kButton,
        kHighlight,
        kTooltipBase,
        kTooltipText,
        kSelected,
        kClickable,
        kXAxis,
        kYAxis,
        kZAxis,
        kWAxis,
        kBlueButton,
        kCount
      };

      /**
      * @brief Applies the global style
      *
      * @param[in] app The application to set the global style for
      */
      static void ApplyStyle(QApplication* app);

      /**
      * @brief Retrieves a color from a provided color role
      *
      * @param[in] role The color role to retrieve
      * @param[in] a The alpha, default = 255
      *
      * @return The retrieved color
      */
      static QColor GetStyleColor(ColorRole role, int a = 255);

      /**
      * @brief Converts a QColor to its respective CSS string
      *
      * @param[in] color The color to convert
      *
      * @return The stringified color as "rgba(r, g, b, a)" string
      */
      static QString GetCSSColor(const QColor& color);

      /**
      * @see Styling::GetStyleColor
      * @see Styling::GetCSSColor
      */
      static QString GetStyleColorCSS(ColorRole role, int a = 255);

      /**
      * @brief Creates a qlineargradient CSS style rule from two colors
      *
      * @param[in] a The first color
      * @param[in] b The second color to interpolate to
      * @param[in] horizontal Is this a horizontal gradient? Default = false
      *
      * @return The style rule
      */
      static QString GradientCSS(
        const QColor& a, 
        const QColor& b, 
        bool horizontal = false);

    private:

      /**
      * @brief The different color roles that exist
      */
      static QColor kColorRoles_[ColorRole::kCount];
    };
  }
}