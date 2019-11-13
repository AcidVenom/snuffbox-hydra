#pragma once

#include <QLineEdit>

namespace snuffbox
{
  namespace editor
  {
    /**
    * @brief Basically just a line edit, but there is supposedly a bug
    *        with the editingFinished signal that it is emitted twice upon
    *        editing completion. This line edit actually checks if the inner
    *        value has been changed
    *
    * @author Daniel Konings
    */
    class PropertyLineEdit : public QLineEdit
    {

      Q_OBJECT;

    public:

      /**
      * @brief Construct through a parent
      *
      * @param[in] parent The parent of this widget, default = nullptr
      */
      PropertyLineEdit(QWidget* parent = nullptr);

      /**
      * @brief Sets the value of this line edit
      *
      * @param[in] value The value to set
      */
      void SetValue(const QString& value);

      /**
      * @return The inner value of this line edit
      */
      QString Value() const;

    signals:

      void ValueChanged(const QString& value);

    private:

      QString old_value_; //!< The old value to compare against
    };
  }
}