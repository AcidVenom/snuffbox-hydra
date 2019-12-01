#include "tools/editor/property-editor/property_vector_edit.h"

#include "tools/editor/property-editor/property_number_edit.h"

#include "tools/editor/application/styling.h"

#include <QLabel>
#include <QHBoxLayout>

#include <algorithm>
#include <limits>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    const int PropertyVectorEdit::kVectorLabelSize_ = 21;
    const int PropertyVectorEdit::kVectorSpacing_ = 5;
    const int PropertyVectorEdit::kVectorEditWidth_ = 90;

    //--------------------------------------------------------------------------
    PropertyVectorEdit::PropertyVectorEdit(int length, QWidget* parent) :
      QWidget(parent),
      length_(length)
    {
      memset(edits_, 0x0, sizeof(QDoubleSpinBox*) * 4);
      memset(old_values_, 0, sizeof(double) * 4);

      length_ = std::max(0, std::min(length_, 4));

      QHBoxLayout* layout = new QHBoxLayout(this);

      const char* label_texts[] =
      {
        "x", "y", "z", "w"
      };

      QString stylesheet =
        "background: %0; color: white; border-radius: 5px;";

      int first_col = Styling::ColorRole::kXAxis;

      for (int i = 0; i < length_; ++i)
      {
        const char* label_text = label_texts[i];

        QLabel* label = new QLabel(this);
        label->setText(label_text);
        label->setAlignment(Qt::AlignCenter);
        label->setFixedSize(kVectorLabelSize_, kVectorLabelSize_);

        QString color = Styling::GetStyleColorCSS(
          static_cast<Styling::ColorRole>(first_col + i));

        label->setStyleSheet(stylesheet.arg(color));

        layout->addWidget(label);

        PropertyNumberEdit* edit = new PropertyNumberEdit(this);
        edit->setFixedWidth(kVectorEditWidth_);

        connect(edit, &PropertyNumberEdit::ValueChanged, this, [=](double val)
        {
          emit ValueChanged(i, val);
        });

        layout->addWidget(edit);

        edits_[i] = edit;
      }

      layout->setContentsMargins(0, 0, 0, 0);
      layout->setSpacing(kVectorSpacing_);

      setLayout(layout);
      setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    }

    //--------------------------------------------------------------------------
    void PropertyVectorEdit::SetValue(const glm::vec4& value)
    {
      for (int i = 0; i < length_; ++i)
      {
        PropertyNumberEdit* spin_box = edits_[i];

        if (spin_box == nullptr)
        {
          return;
        }

        double component_value = static_cast<double>(value[i]);
        old_values_[i] = component_value;
        spin_box->SetValue(component_value);
      }
    }

    //--------------------------------------------------------------------------
    glm::vec4 PropertyVectorEdit::Value() const
    {
      glm::vec4 result = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

      for (int i = 0; i < length_; ++i)
      {
        PropertyNumberEdit* spin_box = edits_[i];

        if (spin_box == nullptr)
        {
          return result;
        }

        result[i] = static_cast<float>(spin_box->Value());
      }

      return result;
    }
  }
}