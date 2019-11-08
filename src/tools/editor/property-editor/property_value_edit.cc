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

      QWidget* frame = new QWidget(this);
      frame->setObjectName(QStringLiteral("PropertyValueEditInner"));
      frame->setStyleSheet(
        "#PropertyValueEditInner {"
        " background: rgb(30, 50, 60);"
        " border-radius: 5px;"
        " border: 1px solid rgb(0, 0, 40);"
      "}");

      QHBoxLayout* row = new QHBoxLayout(this);
      widget_ = CreateWidget();

      QLabel* name_label = new QLabel(this);
      QString name_text = name_;

      QString first = name_text.at(0);
      name_text = first.toUpper() + name_text.mid(1);

      name_label->setText(name_text);
      name_label->setFixedWidth(64);

      row->addWidget(name_label);
      row->addWidget(widget_);

      row->setMargin(10);
      row->setAlignment(Qt::AlignLeft);

      int c = 7;
      row->setContentsMargins(c * 2, c, c * 2, c);

      frame->setLayout(row);
      frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

      QVBoxLayout* main_layout = new QVBoxLayout(this);
      main_layout->addWidget(frame);

      setLayout(main_layout);
      
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
      QHBoxLayout* layout = new QHBoxLayout(frame);

      const char* label_texts[] =
      {
        "x", "y", "z", "w"
      };

      const char* label_colors[] =
      {
        "200, 30, 30",
        "0, 125, 0",
        "0, 125, 255",
        "0, 0, 0"
      };

      QString stylesheet = 
        "background: rgb(%0); color: white; border-radius: 5px;";

      for (int i = 0; i < length; ++i)
      {
        const char* label_text = label_texts[i];

        QLabel* label = new QLabel(frame);
        label->setText(label_text);
        label->setAlignment(Qt::AlignCenter);

        int label_size = 24;
        label->setMinimumWidth(label_size);
        label->setMinimumHeight(label_size);

        label->setStyleSheet(stylesheet.arg(label_colors[i]));

        layout->addWidget(label);

        QLineEdit* edit = new QLineEdit(frame);

        int edit_size = 70;
        edit->setMinimumWidth(edit_size);
        edit->setMaximumWidth(edit_size);

        layout->addWidget(edit);
      }

      layout->setMargin(5);
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