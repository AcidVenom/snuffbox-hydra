#include "tools/editor/property-editor/property_value_edit.h"

#include "tools/editor/property-editor/property_value.h"
#include "tools/editor/property-editor/property_vector_edit.h"
#include "tools/editor/property-editor/property_number_edit.h"
#include "tools/editor/property-editor/property_line_edit.h"

#include "tools/editor/scene-editor/entity_commands.h"
#include "tools/editor/scene-editor/hierarchy_view.h"

#include "tools/editor/application/styling.h"

#include <engine/ecs/entity.h>

#include <QCheckBox>
#include <QComboBox>

#include <QHBoxLayout>
#include <QLabel>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    const int PropertyValueEdit::kSpacing_ = 20;
    const int PropertyValueEdit::kLabelWidth_ = 90;
    const int PropertyValueEdit::kMaxEditWidth_ = 180;

    //--------------------------------------------------------------------------
    PropertyValueEdit::PropertyValueEdit(
      HierarchyView* hierarchy,
      engine::Entity* ent,
      engine::IComponent* component,
      const foundation::String& name,
      const foundation::SharedPtr<PropertyValue>& prop,
      QWidget* parent) :
      QWidget(parent),
      hierarchy_(hierarchy),
      entity_(ent),
      component_(component),
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
        int semicolon = name_text.lastIndexOf(':');
        int first_char_at = semicolon + 1;

        QString first_char = name_text.mid(first_char_at, 1);
        name_text = first_char.toUpper() + name_text.mid(first_char_at + 1);
      }

      name_text = name_text.replace('_', ' ');
      
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
      void* object = GetRawObject();

      size_t required;
      if (prop_->GetRaw(object, nullptr, &required) == true)
      {
        if (prop_->GetRaw(object, new_data, &required) == true)
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

      using PVE = PropertyValueEdit;

      switch (type_)
      {
      case EditTypes::kCheckBox:
        {
          QCheckBox* checkbox = new QCheckBox(this);
          connect(checkbox, &QCheckBox::toggled, this, &PVE::OnCheckboxChanged);
          widget = checkbox;
        }
        break;

      case EditTypes::kComboBox:
        {
          QComboBox* combo = new QComboBox(this);
          connect(
            combo, 
            qOverload<int>(&QComboBox::currentIndexChanged), 
            this, 
            &PVE::OnComboBoxChanged);
          widget = combo;
        }
        break;

      case EditTypes::kNumberEdit:
        {
          PropertyNumberEdit* num = new PropertyNumberEdit(this);
          connect(num, &PropertyNumberEdit::editingFinished, this, [this, num]()
          {
            QSignalBlocker blocker(num);
            OnNumberChanged(num->value());
          });

          widget = num;
          widget->setMaximumWidth(kMaxEditWidth_);
        }
        break;

      case EditTypes::kLineEdit:
        {
          PropertyLineEdit* line = new PropertyLineEdit(this);
          connect(
            line, 
            &PropertyLineEdit::ValueChanged, 
            this, 
            [this](const QString& value)
            {
              OnStringChanged(value);
            });

          widget = line;
          widget->setMaximumWidth(kMaxEditWidth_);
        }
        break;

      case EditTypes::kVec2Edit:
      case EditTypes::kVec3Edit:
      case EditTypes::kVec4Edit:
        {
          PropertyVectorEdit* vec = new PropertyVectorEdit(
            static_cast<int>(type_) - 
            static_cast<int>(EditTypes::kVec2Edit) + 2,
            this);

          connect(
            vec, 
            &PropertyVectorEdit::ValueChanged, 
            this, 
            &PVE::OnVectorChanged);

          widget = vec;
        }
        break;
      }

      if (widget != nullptr)
      {
        QSignalBlocker blocker(widget);
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

      QSignalBlocker blocker(widget_);

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
          static_cast<PropertyLineEdit*>(widget_)->SetValue(value.c_str());
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

    //--------------------------------------------------------------------------
    void* PropertyValueEdit::GetRawObject() const
    {
      return component_ == nullptr ? 
        reinterpret_cast<void*>(entity_) : 
        reinterpret_cast<void*>(component_);
    }

    //--------------------------------------------------------------------------
    PropertyEntityCommand* PropertyValueEdit::CreateSetCommand()
    {
      engine::Components component_type = engine::Components::kCount;
      int component_index = -1;

      if (component_ != nullptr)
      {
        int start = static_cast<int>(engine::Components::kTransform);
        int end = static_cast<int>(engine::Components::kCount);

        for (int c = start; c < end; ++c)
        {
          engine::Components type = static_cast<engine::Components>(c);
          const foundation::Vector<engine::IComponent*>& comps =
            entity_->GetComponents(type);

          for (int i = 0; i < comps.size(); ++i)
          {
            if (comps.at(i) == component_)
            {
              component_type = type;
              component_index = i;
              break;
            }
          }

          if (component_type != engine::Components::kCount)
          {
            break;
          }
        }
      }

      const foundation::UUID& uuid = entity_->uuid();
      foundation::String prop_name = name_.toLatin1().data();

      if (component_index == -1 || component_type == engine::Components::kCount)
      {
        return new PropertyEntityCommand(uuid, hierarchy_, prop_name);
      }

      return new PropertyEntityCommand(
        uuid, 
        hierarchy_, 
        prop_name, 
        component_type, 
        component_index);
    }

    //--------------------------------------------------------------------------
    void PropertyValueEdit::OnCheckboxChanged(bool toggled)
    {
      PropertyEntityCommand* cmd = CreateSetCommand();
      cmd->Set(toggled);

      hierarchy_->PushUndoCommand(cmd);
    }

    //--------------------------------------------------------------------------
    void PropertyValueEdit::OnComboBoxChanged(int index)
    {
      PropertyEntityCommand* cmd = CreateSetCommand();
      cmd->SetComboValue(index);

      hierarchy_->PushUndoCommand(cmd);
    }

    //--------------------------------------------------------------------------
    void PropertyValueEdit::OnNumberChanged(double value)
    {
      PropertyEntityCommand* cmd = CreateSetCommand();
      cmd->Set(value);

      hierarchy_->PushUndoCommand(cmd);
    }

    //--------------------------------------------------------------------------
    void PropertyValueEdit::OnStringChanged(const QString& value)
    {
      PropertyEntityCommand* cmd = CreateSetCommand();

      foundation::String str = value.toLatin1().data();
      cmd->Set(str);

      hierarchy_->PushUndoCommand(cmd);
    }

    //--------------------------------------------------------------------------
    void PropertyValueEdit::OnVectorChanged(int comp, double value)
    {
      glm::vec4 vec = static_cast<PropertyVectorEdit*>(widget_)->Value();

      PropertyEntityCommand* cmd = CreateSetCommand();
      
      switch (type_)
      {
      case EditTypes::kVec2Edit:
        cmd->Set(glm::vec2(vec.x, vec.y));
        break;

      case EditTypes::kVec3Edit:
        cmd->Set(glm::vec3(vec.x, vec.y, vec.z));
        break;

      case EditTypes::kVec4Edit:
        cmd->Set(vec);
        break;

      default:
        cmd->Set(vec);
        break;
      }

      hierarchy_->PushUndoCommand(cmd);
    }
  }
}