#pragma once

#include <QMessageBox>

namespace snuffbox
{
  namespace editor
  {
    /**
    * @brief Used to show wider message boxes than the regular QMessageBox
    *
    * @author Daniel Konings
    */
    class MessageBox
    {

    public:

      /**
      * @brief Shows the message box with specified properties
      *
      * @param[in] icon The icon to display
      * @param[in] title The title of the message box
      * @param[in] text The text within the message box
      * @param[in] width The width of the message box
      * @param[in] buttons The buttons to show
      */
      static void Show(
        QMessageBox::Icon icon,
        const QString& title,
        const QString& text,
        int width = 640,
        QMessageBox::Button buttons = QMessageBox::Button::Ok);
    };
  }
}