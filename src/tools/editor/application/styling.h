#pragma once

class QApplication;

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
      *
      * @return The stringified color as "rgb(r, g, b)" string
      */
      static const char* GetStyleColor(ColorRole role);

    private:

      /**
      * @brief The different color roles that exist
      */
      static const char* kColorRoles_[ColorRole::kCount];
    };
  }
}