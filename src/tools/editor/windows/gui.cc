#include "tools/editor/windows/gui.h"
#include "tools/editor/definitions/editor_colors.h"

#include <foundation/memory/memory.h>
#include <foundation/auxiliary/logger.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qvalidator.h>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    GUI::GUI() :
      current_layout_(nullptr),
      num_started_(0)
    {
      ResetColors();
    }

    //--------------------------------------------------------------------------
    void GUI::SetForegroundColor(const QColor& color)
    {
      current_color_.foreground = color;
    }

    //--------------------------------------------------------------------------
    void GUI::SetBackgroundColor(const QColor& color)
    {
      current_color_.background = color;
    }

    //--------------------------------------------------------------------------
    void GUI::ResetForegroundColor()
    {
      const QPalette& p = EditorColors::DefaultPalette();
      current_color_.foreground = p.color(QPalette::ColorRole::Text);
    }

    //--------------------------------------------------------------------------
    void GUI::ResetBackgroundColor()
    {
      const QPalette& p = EditorColors::DefaultPalette();
      current_color_.background = p.color(QPalette::ColorRole::Base);
    }

    //--------------------------------------------------------------------------
    void GUI::ResetColors()
    {
      ResetForegroundColor();
      ResetBackgroundColor();
    }

    //--------------------------------------------------------------------------
    void GUI::StartLayout(LayoutStyle layout)
    {
      Layout* new_layout = foundation::Memory::Construct<Layout>(
          &foundation::Memory::default_allocator());

      new_layout->style = layout;
      new_layout->prev = current_layout_;
      current_layout_ = new_layout;
      
      switch (layout)
      {
      case LayoutStyle::kHorizontal:
        current_layout_->qlayout = new QHBoxLayout();
        break;

      case LayoutStyle::kVertical:
        current_layout_->qlayout = new QVBoxLayout();
        break;
      }

      ++num_started_;
    }

    //--------------------------------------------------------------------------
    void GUI::SetSpacing(int s)
    {
      if (current_layout_ == nullptr)
      {
        return;
      }

      current_layout_->qlayout->setSpacing(s);
    }

    //--------------------------------------------------------------------------
    void GUI::SetMargins(int left, int top, int right, int bottom)
    {
      if (current_layout_ == nullptr)
      {
        return;
      }

      current_layout_->qlayout->setContentsMargins(left, top, right, bottom);
    }

    //--------------------------------------------------------------------------
    QLayout* GUI::EndLayout()
    {
      if (current_layout_ == nullptr || num_started_ == 0)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kEditor,
          foundation::LogSeverity::kWarning,
          "Attempted to end more layouts in the GUI utility than were started");

        return nullptr;
      }

      Layout* prev = current_layout_->prev;
      QLayout* ret = current_layout_->qlayout;

      if (prev != nullptr)
      {
        prev->qlayout->addItem(current_layout_->qlayout);
      }

      foundation::Memory::Destruct<Layout>(current_layout_);
      current_layout_ = prev;

      --num_started_;

      return ret;
    }

    //--------------------------------------------------------------------------
    void GUI::Label(const char* name)
    {
      if (current_layout_ == nullptr)
      {
        return;
      }

      QLabel* label = new QLabel();
      label->setText(name);
      
      AddWidget(label);
    }

    //--------------------------------------------------------------------------
    void GUI::Checkbox(
      bool value, 
      ChangeCallback<bool> on_changed)
    {
      if (current_layout_ == nullptr)
      {
        return;
      }

      QCheckBox* box = new QCheckBox();
      box->setChecked(value);

      QObject::connect(box, &QCheckBox::stateChanged, [=](int)
      {
        if (on_changed != nullptr)
        {
          on_changed(box->isChecked());
        }
      });

      AddWidget(box);
    }

    //--------------------------------------------------------------------------
    void GUI::TextField(
      const char* value,
      ChangeCallback<const QString&> on_changed)
    {
      if (current_layout_ == nullptr)
      {
        return;
      }

      QLineEdit* edit = new QLineEdit();
      edit->setText(value);

      QObject::connect(edit, &QLineEdit::editingFinished, [=]()
      {
        if (on_changed != nullptr)
        {
          on_changed(edit->text());
        }
      });

      AddWidget(edit);
    }

    //--------------------------------------------------------------------------
    QLineEdit* GUI::NumberField(
      double value,
      bool floor,
      ChangeCallback<double> on_changed)
    {
      if (current_layout_ == nullptr)
      {
        return nullptr;
      }

      QLineEdit* edit = new QLineEdit();
      edit->setValidator(new QDoubleValidator());

      std::string snum = std::to_string(value);

      size_t pos = snum.find_first_of('.');

      if (pos == std::string::npos)
      {
        pos = snum.find_first_of(',');
      }

      size_t erase_from = pos;

      ++pos;

      while (pos != snum.size())
      {
        if (snum.at(pos) != '0')
        {
          erase_from = ++pos;
          continue;
        }

        ++pos;
      }

      snum.erase(snum.begin() + erase_from, snum.end());

      edit->setText(snum.c_str());

      QObject::connect(edit, &QLineEdit::editingFinished, [=]()
      {
        if (on_changed != nullptr)
        {
          std::string s = edit->text().toStdString();
          on_changed(atof(s.c_str()));
        }
      });

      AddWidget(edit);

      return edit;
    }

    //--------------------------------------------------------------------------
    void GUI::Button(
      const char* text,
      const std::function<void()>& on_click)
    {
      if (current_layout_ == nullptr)
      {
        return;
      }

      QPushButton* button = new QPushButton();
      button->setText(text);

      QObject::connect(button, &QPushButton::clicked, on_click);

      AddWidget(button);
    }

    //--------------------------------------------------------------------------
    void GUI::HorizontalLine(const QColor& color)
    {
      QWidget* line = new QWidget();
      line->setFixedHeight(1);
      line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

      QString scolor = EditorColors::ColorToCSS(color);
      line->setStyleSheet("background-color: " + scolor + ";");

      AddWidget(line);
    }

    //--------------------------------------------------------------------------
    void GUI::AddWidget(QWidget* widget)
    {
      if (current_layout_ == nullptr)
      {
        return;
      }
      
      QPalette palette;
      palette.setColor(widget->foregroundRole(), current_color_.foreground);
      palette.setColor(widget->backgroundRole(), current_color_.background);
      palette.setColor(QPalette::Text, current_color_.foreground);

      widget->setPalette(palette);

      current_layout_->qlayout->addWidget(widget);
    }

    //--------------------------------------------------------------------------
    QWidget* GUI::EndAsWidget()
    {
      QLayout* layout = EndLayout();

      if (layout == nullptr)
      {
        return nullptr;
      }

      QWidget* widget = new QWidget();
      widget->setLayout(layout);

      return widget;
    }

    //--------------------------------------------------------------------------
    GUI::~GUI()
    {
      if (num_started_ > 0)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kEditor,
          foundation::LogSeverity::kWarning,
          "Not all GUI utility layouts were properly closed\n\
          \tCleaning up {0} layout(s) automatically",
          num_started_);

        while (current_layout_ != nullptr)
        {
          Layout* prev = current_layout_->prev;
          foundation::Memory::Destruct<Layout>(current_layout_);
          current_layout_ = prev;
        }
      }
    }
  }
}
