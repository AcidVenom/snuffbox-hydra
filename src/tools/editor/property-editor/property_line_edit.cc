#include "tools/editor/property-editor/property_line_edit.h"

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    PropertyLineEdit::PropertyLineEdit(QWidget* parent) :
      QLineEdit(parent),
      old_value_("")
    {
      connect(this, &QLineEdit::editingFinished, this, [=]()
      {
        clearFocus();

        QString value = text();
        if (value == old_value_)
        {
          return;
        }

        old_value_ = value;
        emit ValueChanged(value);
      });
    }

    //--------------------------------------------------------------------------
    void PropertyLineEdit::SetValue(const QString& value)
    {
      setText(value);
      old_value_ = value;
    }

    //--------------------------------------------------------------------------
    QString PropertyLineEdit::Value() const
    {
      return text();
    }
  }
}