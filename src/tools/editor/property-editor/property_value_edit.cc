#include "tools/editor/property-editor/property_value_edit.h"

#include "tools/editor/property-editor/property_value.h"
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
    const size_t PropertyValueEdit::kMaxDataSize_;
    const int PropertyValueEdit::kSpacing_ = 20;
    const int PropertyValueEdit::kLabelWidth_ = 80;
    const int PropertyValueEdit::kMaxEditWidth_ = 180;
    const int PropertyValueEdit::kVectorLabelSize_ = 21;
    const int PropertyValueEdit::kVectorSpacing_ = 5;

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
      row_layout->addWidget(CreateWidget());

      row->setLayout(row_layout);
      setLayout(main_layout);
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
          changed = memcmp(old_data_, new_data, required) == 0;
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
      case EditTypes::kLineEdit:
        widget = new QLineEdit(this);
        widget->setMaximumWidth(kMaxEditWidth_);
        break;

      case EditTypes::kVec2Edit:
      case EditTypes::kVec3Edit:
      case EditTypes::kVec4Edit:
        widget = CreateVectorEdit(
          static_cast<int>(type_) - static_cast<int>(EditTypes::kVec2Edit) + 2);
        break;
      }

      if (widget != nullptr)
      {
        widget->setContentsMargins(0, 0, 0, 0);
      }

      return widget;
    }

    //--------------------------------------------------------------------------
    QWidget* PropertyValueEdit::CreateVectorEdit(int length)
    {
      QWidget* frame = new QWidget(this);
      QHBoxLayout* layout = new QHBoxLayout(frame);

      const char* label_texts[] =
      {
        "x", "y", "z", "w"
      };

      QString stylesheet = 
        "background: %0; color: white; border-radius: 5px;";

      int first_col = Styling::ColorRole::kXAxis;

      for (int i = 0; i < length; ++i)
      {
        const char* label_text = label_texts[i];

        QLabel* label = new QLabel(frame);
        label->setText(label_text);
        label->setAlignment(Qt::AlignCenter);
        label->setFixedSize(kVectorLabelSize_, kVectorLabelSize_);

        QString color = Styling::GetStyleColorCSS(
          static_cast<Styling::ColorRole>(first_col + i));

        label->setStyleSheet(stylesheet.arg(color));

        layout->addWidget(label);

        QLineEdit* edit = new QLineEdit(frame);
        edit->setFixedWidth(kMaxEditWidth_ * 0.5);

        layout->addWidget(edit);
      }

      layout->setContentsMargins(0, 0, 0, 0);
      layout->setSpacing(kVectorSpacing_);

      frame->setLayout(layout);
      frame->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

      return frame;
    }

    //--------------------------------------------------------------------------
    void PropertyValueEdit::UpdateValue(const uint8_t* data)
    {
      memcpy(old_data_, data, kMaxDataSize_);
    }
  }
}