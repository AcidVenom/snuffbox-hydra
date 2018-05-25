#pragma once

#include "tools/editor/definitions/color_format.h"

class QLayout;
class QWidget;

namespace snuffbox
{
  namespace editor
  {
    /**
    * @brief Used to wrap around the Qt widget system to dynamically create
    *        GUIs
    *
    * Done a bit like Unity's GUI system, but without the bi-directional
    * values and with callbacks instead.
    *
    * @author Daniel Konings
    */
    class GUI
    {

    protected:

      /**
      * @brief Used to keep track of the currently entered layout
      *
      * @author Daniel Konings
      */
      struct Layout
      {
        QLayout* qlayout; //!< The underlying actual Qt layout
        Layout* prev; //!< The previous layout that we were at
      };

    public:

      /**
      * @brief The different types of layout styles that exist
      */
      enum class LayoutStyle
      {
        kHorizontal, //!< A QHBoxLayout
        kVertical //!< A QVBoxLayout
      };

      /**
      * @brief Default constructor, initializes all values to their
      *        defaults
      */
      GUI();

      /**
      * @brief Sets the current foreground color
      *
      * @param[in] color The color to set
      *
      * @remarks Please note not all widgets support a foreground color,
      *          but for the ones that do this gets applied
      */
      void SetForegroundColor(const QColor& color);

      /**
      * @brief Sets the current background color
      *
      * @param[in] color The color to set
      *
      * @remarks Please note not all widgets support a background color,
      *          but for the ones that do this gets applied
      */
      void SetBackgroundColor(const QColor& color);

      /**
      * @brief Resets the foreground color to its default value
      */
      void ResetForegroundColor();

      /**
      * @brief Resets the background color to its default value
      */
      void ResetBackgroundColor();

      /**
      * @brief Resets both the foreground and the background color
      */
      void ResetColors();

      /**
      * @brief Starts a new layout
      *
      * @param[in] layout The new layout type
      */
      void StartLayout(LayoutStyle layout);

      /**
      * @brief Sets the spacing between the elements in the layout
      *
      * @param[in] s The spacing to apply for this layout
      */
      void SetSpacing(int s);

      /**
      * @brief Sets the margins between the elements in the layout
      *
      * @param[in] left The margin to the left of each element
      * @param[in] top The margin to the top of each element
      * @param[in] right The margin to the right of each element
      * @param[in] bottom The margin to the bottom of each element
      */
      void SetMargins(int left, int top, int right, int bottom);

      /**
      * @brief Ends the layout and returns the pointer to the layout that was
      *        closed
      *
      * If any layout is not ended, the GUI will throw a warning during its
      * destruction. This means some QLayouts will actually leak memory,
      * but the underlying Layout structure is cleaned up appropriately.
      *
      * @return The closed layout
      */
      QLayout* EndLayout();

      /**
      * @brief Adds a label field to the current layout
      *
      * @param[in] name The text value of the label
      */
      void Label(const char* name);

      /**
      * @brief Adds a checkbox to the current layout
      *
      * @param[in] value The checked value of the checkbox
      */
      void Checkbox(bool value);

      /**
      * @brief Adds a text field to the current layout
      *
      * @param[in] value The text value of the text field
      */
      void TextField(const char* value);

      /**
      * @brief Adds a raw widget to the layout, which the user can define
      *        themselves
      *
      * @remarks This method is used internally to apply colors to the
      *          supported GUI widgets
      *
      * The current color palette is applied and the widget is added to the
      * current layout.
      *
      * @param[in] widget The widget to add
      */
      void AddWidget(QWidget* widget);

      /**
      * @brief Destructor, checks if all layouts were closed
      *
      * @see GUI::EndLayout
      */
      ~GUI();

    private:

      ColorFormat current_color_; //!< The current color format

      Layout* current_layout_; //!< The current layout
      int num_started_; //!< The number of layouts that were started

      
    };
  }
}