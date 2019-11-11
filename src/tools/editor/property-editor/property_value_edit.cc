#include "tools/editor/property-editor/property_value_edit.h"

#include "tools/editor/property-editor/property_value.h"
#include "tools/editor/property-editor/property_vector_edit.h"
#include "tools/editor/property-editor/property_number_edit.h"

#include "tools/editor/application/styling.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>

#include <QHBoxLayout>
#include <QLabel>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    const int PropertyValueEdit::kSpacing_ = 20;
    const int PropertyValueEdit::kLabelWidth_ = 80;
    const int PropertyValueEdit::kMaxEditWidth_ = 180;

    //--------------------------------------------------------------------------
    PropertyValueEdit::PropertyValueEdit(
      void* object,
      const foundation::String& name,
      const foundation::SharedPtr<PropertyValue>& prop,
      QWidget* parent) :
      QWidget(parent),
      object_(object),
      name_(name.c_str()),
      prop_(prop),
      type_(EditTypes::kCount),
      widget_(nullptr)
    {
      memset(old_data_, 0, kMaxDataSize_);
      setObjectName(QStringLiteral("PropertyValueEditInner"));

      if (prop->IsBoolean() == true)
      {
        type_ = EditTypes::kCheckBox;
      }
      else if (prop->IsEnum() == true)
      {
        type_ = EditTypes::kComboBox;
      }
      else if (prop->IsNumber() == true)
      {
        type_ = EditTypes::kNumberEdit;
      }
      else if (prop->IsString() == true || prop->IsUUID() == true)
      {
        type_ = EditTypes::kLineEdit;
      }
      
      int vec_start = static_cast<int>(EditTypes::kVec2Edit);
      int vec_end = static_cast<int>(EditTypes::kVec4Edit);

      for (int i = vec_start; i <= vec_end; ++i)
      {
        int current = i - vec_start;
        if (prop->IsVector(current + 2) == true)
        {
          type_ = static_cast<EditTypes>(i);
        }
      }

      QVBoxLayout* main_layout = new QVBoxLayout(this);

      QWidget* row = new QWidget(this);

      QString bg = Styling::GetStyleColorCSS(Styling::ColorRole::kBackground);

      row->setObjectName(QStringLiteral("PropertyEditRow"));
      row->setStyleSheet(
        QString("#PropertyEditRow { background: %0; border-radius: 5px; }")
        .arg(bg));

      row->setContentsMargins(0, 0, 0, 0);

      QHBoxLayout* row_layout = new QHBoxLayout(row);
      row_layout->setSpacing(kSpacing_);
      row_layout->setAlignment(Qt::AlignLeft);

      main_layout->addWidget(row);
      main_layout->setContentsMargins(0, 0, 0, 0);
      
      setContentsMargins(0, 0, 0, 0);
      setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

      QLabel* name_label = new QLabel(row);

      QString name_text = name_;

      if (name_text.size() > 0)
      {
        QString first_char = name_text.mid(0, 1);
        name_text = first_char.toUpper() + name_text.mid(1);
      }
      
      name_label->setText(name_text);
      name_label->setFixedWidth(kLabelWidth_);
      name_label->setContentsMargins(0, 0, 0, 0);

      row_layout->addWidget(name_label);

      widget_ = CreateWidget();
      row_layout->addWidget(widget_);

      row->setLayout(row_layout);
      setLayout(main_layout);

      Update();
    }

    //--------------------------------------------------------------------------
    void PropertyValueEdit::Update()
    {
      uint8_t new_data[kMaxDataSize_];
      memset(new_data, 0, kMaxDataSize_);

      bool changed = false;

      size_t required;
      if (prop_->GetRaw(object_, nullptr, &required) == true)
      {
        if (prop_->GetRaw(object_, new_data, &required) == true)
        {
          changed = memcmp(old_data_, new_data, required) != 0;
        }
      }

      if (changed == true)
      {
        UpdateValue(new_data);
      }
    }

    //--------------------------------------------------------------------------
    QWidget* PropertyValueEdit::CreateWidget()
    {
      QWidget* widget = nullptr;

      switch (type_)
      {
      case EditTypes::kCheckBox:
        widget = new QCheckBox(this);
        break;

      case EditTypes::kComboBox:
        widget = new QComboBox(this);
        break;

      case EditTypes::kNumberEdit:
        widget = new PropertyNumberEdit(this);
        widget->setMaximumWidth(kMaxEditWidth_);
        break;

      case EditTypes::kLineEdit:
        widget = new QLineEdit(this);
        widget->setMaximumWidth(kMaxEditWidth_);
        break;

      case EditTypes::kVec2Edit:
      case EditTypes::kVec3Edit:
      case EditTypes::kVec4Edit:
        widget = new PropertyVectorEdit(
          static_cast<int>(type_) - static_cast<int>(EditTypes::kVec2Edit) + 2,
          this);
        break;
      }

      if (widget != nullptr)
      {
        widget->setContentsMargins(0, 0, 0, 0);

        if (type_ == EditTypes::kComboBox)
        {
          const foundation::Vector<foundation::String>& combo_values =
            prop_->combo_box_values();

          QComboBox* combo_box = static_cast<QComboBox*>(widget);
          for (int i = 0; i < combo_values.size(); ++i)
          {
            combo_box->addItem(combo_values.at(i).c_str());
          }
        }
      }

      return widget;
    }

    //--------------------------------------------------------------------------
    void PropertyValueEdit::UpdateValue(const uint8_t* data)
    {
      memcpy(old_data_, data, kMaxDataSize_);

      if (widget_ == nullptr)
      {
        return;
      }

      switch (type_)
      {
      case EditTypes::kCheckBox:
        {
          bool checked = *reinterpret_cast<const bool*>(data);
          static_cast<QCheckBox*>(widget_)->setChecked(checked);
        }
        break;

      case EditTypes::kComboBox:
        {
          EnumProperty enum_idx = *reinterpret_cast<const EnumProperty*>(data);
          static_cast<QComboBox*>(widget_)->setCurrentIndex(enum_idx);
        }
        break;

      case EditTypes::kNumberEdit:
        {
          double number = *reinterpret_cast<const double*>(data);
          static_cast<PropertyNumberEdit*>(widget_)->setValue(number);
        }
        break;

      case EditTypes::kLineEdit:
        {
          foundation::String value = reinterpret_cast<const char*>(data);
          static_cast<QLineEdit*>(widget_)->setText(value.c_str());
        }
        break;

      case EditTypes::kVec2Edit:
      case EditTypes::kVec3Edit:
      case EditTypes::kVec4Edit:
        {
          int start = static_cast<int>(EditTypes::kVec2Edit);
          int length = static_cast<int>(type_) - start + 2;

          glm::vec4 value = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

          for (int i = 0; i < length; ++i)
          {
            value[i] = reinterpret_cast<const float*>(data)[i];
          }

          static_cast<PropertyVectorEdit*>(widget_)->SetValue(value);
        }
      }
    }
  }
}