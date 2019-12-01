#pragma once

#include <QDoubleSpinBox>

namespace snuffbox
{
  namespace editor
  {
    /**
    * @brief A QDoubleSpinBox with limits of -max float < 0 < max float
    *        without trailing zeros in its string representation and 
    *        step increments of 0.01f
    *
    * @remarks This class also makes sure '.' are always used as decimal
    *          separator. Another nice thing is that the 'editingFinished'
    *          signal will not be emitted twice from this..
    *
    * @author Daniel Konings
    */
    class PropertyNumberEdit : public QDoubleSpinBox
    {

      Q_OBJECT;

    public:

      /**
      * @brief Construct through a parent
      */
      PropertyNumberEdit(QWidget* parent = nullptr);

    protected:

      /**
      * @brief Used to remove the trailing zeros off of the localized string
      *        value
      *
      * @param[in] value The value to convert
      *
      * @return The stringified double value
      */
      QString textFromValue(double value) const override;

    public:

      /**
      * @brief Sets the value of this number edit
      *
      * @param[in] val The value to set
      */
      void SetValue(double val);

      /**
      * @return The value of this number edit
      */
      double Value() const;

    signals:

      /**
      * @brief Emitted when the inner value has been changed
      *
      * @param[in] value The new value
      */
      void ValueChanged(double value);

    private:

      double old_value_; //!< The inner value to compare with

      static const int kPrecision_; //!< The precision of decimals
      static const double kStepSize_; //!< The step size increment
    };
  }
}