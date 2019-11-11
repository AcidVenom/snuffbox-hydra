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
    *          separator
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

    private:

      static const int kPrecision_; //!< The precision of decimals
      static const double kStepSize_; //!< The step size increment
    };
  }
}