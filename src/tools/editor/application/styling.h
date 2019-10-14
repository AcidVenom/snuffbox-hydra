#pragma once

class QApplication;

#include <QString>

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
        kSelected,
        kClickable,
        kHighlight,
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
      * @param[in] a The alpha, default = 1.0
      *
      * @return The stringified color as "rgba(r, g, b, a)" string
      */
      static QString GetStyleColor(ColorRole role, float a = 1.0f);

    private:

      /**
      * @brief The different color roles that exist
      */
      static const char* kColorRoles_[ColorRole::kCount];
    };
  }
}