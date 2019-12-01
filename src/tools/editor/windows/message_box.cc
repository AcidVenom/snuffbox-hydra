#include "tools/editor/windows/message_box.h"

#include <QSpacerItem>
#include <QGridLayout>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    void MessageBox::Show(
      QMessageBox::Icon icon,
      const QString& title,
      const QString& text,
      int width,
      QMessageBox::Button buttons)
    {
      QMessageBox msg;
      msg.setText(title);
      msg.setInformativeText(text);
      msg.setStandardButtons(buttons);
      msg.setDefaultButton(static_cast<QMessageBox::Button>(buttons << 0));
      msg.setIcon(icon);

      QString window_title = "Snuffbox Editor - ";
      switch (icon)
      {
      case QMessageBox::Icon::Information:
        window_title += "Info";
        break;

      case QMessageBox::Icon::Question:
        window_title += "Are you sure?";
        break;

      case QMessageBox::Icon::Warning:
        window_title += "Warning";
        break;

      case QMessageBox::Icon::Critical:
        window_title += "Error";
        break;

      default:
        window_title.remove(window_title.size() - 1, 3);
        break;
      }

      msg.setWindowTitle(window_title);

      QSpacerItem* spacer = new QSpacerItem(
        width * 0.5,
        0, 
        QSizePolicy::Minimum, 
        QSizePolicy::Expanding);

      QGridLayout* layout = static_cast<QGridLayout*>(msg.layout());
      layout->addItem(
        spacer, 
        layout->rowCount(), 
        0,
        1,
        layout->columnCount());

      // Hack to dodge a Qt error message..?
      msg.setFixedSize(width, msg.height());

      msg.exec();
    }
  }
}