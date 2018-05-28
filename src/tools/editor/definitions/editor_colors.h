#pragma once

#include "tools/editor/definitions/color_format.h"

#include <foundation/definitions/logging.h>

class QPalette;

namespace snuffbox
{
  namespace editor
  {
    /**
    * @brief Used to retrieve default editor colors throughout the editor
    *
    * @author Daniel Konings
    */
    class EditorColors
    {

    public:

      /**
      * @return The default "steam" palette, based on both Steam and the old
      *         version of Snuffbox
      */
      static const QPalette& DefaultPalette();

      /**
      * @return The default dock color, in regards to the "steam" palette
      */
      static const QColor& DockColor();

      /**
      * @brief The different console color formats per severity
      *
      * @param[in] sev The severity to convert
      *
      * @return The color format in correspondance to the severity
      */
      static ColorFormat ConsoleColor(foundation::LogSeverity sev);

      /**
      * @return The blue button color, used anywhere for blue buttons
      */
      static const QColor& BlueButton();

      /**
      * @brief Converts a color to an 'rgb(r, g, b)' string
      *
      * @param[in] color The color to convert
      *
      * @return The converted color
      */
      static QString ColorToCSS(const QColor& color);
    };
  }
}