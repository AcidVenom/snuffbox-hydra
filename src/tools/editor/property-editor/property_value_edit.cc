#include "tools/editor/property-editor/property_value_edit.h"

#include "tools/editor/property-editor/property_value.h"

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

    //--------------------------------------------------------------------------
    PropertyValueEdit::PropertyValueEdit(
      PropertyView* view,
      void* object,
      const foundation::String& name,
      const foundation::SharedPtr<PropertyValue>& prop,
      QWidget* parent) :
      QWidget(parent),
      view_(view),
      object_(object),
      name_(name.c_str()),
      prop_(prop),
      type_(EditTypes::kCount),
      widget_(nullptr)
    {
      memset(old_data_, 0, kMaxDataSize_);

      setObjectName(QStringLiteral("PropertyValueEdit"));

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

      QHBoxLayout* row = new QHBoxLayout();
      widget_ = CreateWidget();

      QLabel* name_label = new QLabel(this);
      name_label->setText(name_);

      row->addWidget(name_label);
      row->addWidget(widget_);

      row->setMargin(20);

      setLayout(row);

      row->setAlignment(Qt::AlignLeft);
      
      setStyleSheet("background: rgb(255, 0, 0);");
      setAutoFillBackground(true);
      setAttribute(Qt::WA_StyleSheet);
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
        break;

      case EditTypes::kVec2Edit:
      case EditTypes::kVec3Edit:
      case EditTypes::kVec4Edit:
        widget = CreateVectorEdit(
          static_cast<int>(type_) - static_cast<int>(EditTypes::kVec2Edit) + 2);
        break;
      }

      return widget;
    }

    //--------------------------------------------------------------------------
    QWidget* PropertyValueEdit::CreateVectorEdit(int length)
    {
      QWidget* frame = new QWidget(this);
      QHBoxLayout* layout = new QHBoxLayout();

      const char* label_texts[] =
      {
        "x", "y", "z", "w"
      };

      const char* label_colors[] =
      {
        "255, 0, 0",
        "0, 255, 0",
        "0, 0, 255",
        "0, 0, 0"
      };

      QString stylesheet = "background: rgb(%0); color: white;";

      for (int i = 0; i < length; ++i)
      {

        const char* label_text = label_texts[i];

        QLabel* label = new QLabel(frame);
        label->setText(label_text);

        int label_size = 16;
        label->setMinimumWidth(label_size);
        label->setMaximumWidth(label_size);

        label->setStyleSheet(stylesheet.arg(label_colors[i]));

        layout->addWidget(label);

        QLineEdit* edit = new QLineEdit(frame);

        int edit_size = 64;
        edit->setMinimumWidth(edit_size);
        edit->setMaximumWidth(edit_size);

        layout->addWidget(edit);
      }

      layout->setMargin(5);
      frame->setLayout(layout);

      return frame;
    }

    //--------------------------------------------------------------------------
    void PropertyValueEdit::UpdateValue(const uint8_t* data)
    {
      memcpy(old_data_, data, kMaxDataSize_);
    }
  }
}