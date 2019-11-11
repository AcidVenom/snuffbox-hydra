#include "tools/editor/property-editor/property_number_edit.h"

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    const int PropertyNumberEdit::kPrecision_ = 7;
    const double PropertyNumberEdit::kStepSize_ = 0.01;

    //--------------------------------------------------------------------------
    PropertyNumberEdit::PropertyNumberEdit(QWidget* parent) :
      QDoubleSpinBox(parent)
    {
      float float_max = std::numeric_limits<float>::max();
      setMinimum(static_cast<double>(-float_max));
      setMaximum(static_cast<double>(float_max));
      setDecimals(kPrecision_);
      setLocale(QLocale::English);
      setSingleStep(kStepSize_);
    }

    //--------------------------------------------------------------------------
    QString PropertyNumberEdit::textFromValue(double value) const
    {
      QString stringified =
        locale().toString(value, 'g', QLocale::FloatingPointShortest);

      return stringified.replace(',', "");
    }
  }
}