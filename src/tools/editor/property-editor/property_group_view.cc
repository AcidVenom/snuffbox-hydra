#include "tools/editor/property-editor/property_group_view.h"

#include "tools/editor/property-editor/property_value_edit.h"

#include "tools/editor/scene-editor/entity_commands.h"
#include "tools/editor/scene-editor/hierarchy_view.h"

#include "tools/editor/application/styling.h"

#include <foundation/containers/function.h>

#include <engine/ecs/entity.h>

#include <QFrame>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    const int PropertyGroupView::kHeaderHeight_ = 23;
    const int PropertyGroupView::kHeaderPadding_ = 4;
    const int PropertyGroupView::kInnerPadding_ = 5;
    const int PropertyGroupView::kInnerSpacing_ = 2;

    //--------------------------------------------------------------------------
    class ClickableLabel : public QLabel
    {

    public:

      //------------------------------------------------------------------------
      ClickableLabel(
        QWidget* parent, 
        const foundation::Function<void(ClickableLabel*)>& cb) : 
        QLabel(parent),
        callback_(cb)
      {

      }

      //------------------------------------------------------------------------
      void mouseReleaseEvent(QMouseEvent* evt) override
      {
        if (callback_ != nullptr)
        {
          callback_(this);
        }
      }

    private:

      foundation::Function<void(ClickableLabel*)> callback_;
    };

    //--------------------------------------------------------------------------
    PropertyGroupView::PropertyGroupView(
      HierarchyView* hierarchy,
      const QString& header,
      const PropertyMap& map,
      engine::Entity* entity,
      engine::IComponent* component,
      engine::Components component_type,
      int component_index,
      QWidget* parent) :
      QWidget(parent)
    {
      const int ph = kHeaderPadding_;
      const int pi = kInnerPadding_;

      QVBoxLayout* edit_layout = new QVBoxLayout(this);
      edit_layout->setContentsMargins(0, 0, 0, 0);

      QWidget* inner = new QWidget(this);
      inner->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
      inner->setContentsMargins(0, 0, 0, 0);
      
      QVBoxLayout* inner_layout = new QVBoxLayout(inner);

      inner_layout->setContentsMargins(pi, pi, pi, pi * 2);
      inner_layout->setSpacing(kInnerSpacing_);
      inner->setLayout(inner_layout);

      setContentsMargins(0, 0, 0, 0);
      setLayout(edit_layout);
      setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

      QWidget* top_bar = new QWidget(this);

      QLabel* group_label = new QLabel(top_bar);
      group_label->setText(header);
      group_label->setFixedHeight(kHeaderHeight_);
      group_label->setContentsMargins(0, 0, 0, 0);

      QColor bg_col_a = Styling::GetStyleColor(Styling::ColorRole::kButton);
      QColor bg_col_b = bg_col_a.darker(125);

      QPushButton* remove_button = nullptr;
      if (
        component != nullptr &&
        component_type != engine::Components::kTransform)
      {
        remove_button = new QPushButton(group_label);

        connect(remove_button, &QPushButton::released, this, [=]()
        {
          RemoveComponentEntityCommand* cmd = new RemoveComponentEntityCommand(
            entity->uuid(),
            hierarchy,
            component_type,
            component_index);

          hierarchy->PushUndoCommand(cmd);
        });

        remove_button->setText("X");
        remove_button->setFixedWidth(24);
        remove_button->setStyleSheet("background: " +
          Styling::GetStyleColorCSS(Styling::ColorRole::kBlueButton));
      }

      ClickableLabel* expand_toggle = new ClickableLabel(
        group_label, 
        [=](ClickableLabel* self)
        {
          bool toggle = inner->isVisible() == false;
          inner->setVisible(toggle);

          QString text = toggle == true ? "-" : "+";
          self->setText(text);
        });

      expand_toggle->setText("-");
      expand_toggle->setStyleSheet(
        "QLabel {"
          "border: 1px solid gray;"
          "color: gray;"
        "}"
        "QLabel:hover {"
          "border: 1px solid white;"
          "color: white;"
        "}");

      expand_toggle->setAlignment(Qt::AlignCenter);
      expand_toggle->setContentsMargins(0, 0, 0, 0);

      int toggle_size = kHeaderHeight_ - ph;
      expand_toggle->setFixedSize(toggle_size, toggle_size);

      QHBoxLayout* top_bar_layout = new QHBoxLayout(top_bar);

      top_bar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

      QString gradient = Styling::GradientCSS(bg_col_a, bg_col_b);
      QString style = "background: %0;";

      top_bar->setStyleSheet(style.arg(gradient));
      top_bar->setContentsMargins(0, 0, 0, 0);

      top_bar_layout->addWidget(group_label);

      if (remove_button != nullptr)
      {
        top_bar_layout->addWidget(remove_button, 1, Qt::AlignRight);
      }

      top_bar_layout->addWidget(expand_toggle, 0, Qt::AlignRight);
      top_bar_layout->setContentsMargins(ph * 2, ph, ph * 2, ph);

      edit_layout->addWidget(top_bar);
      edit_layout->addWidget(inner);

      for (
        PropertyMap::const_iterator it = map.begin();
        it != map.end();
        ++it)
      {
        PropertyValueEdit* edit = new PropertyValueEdit(
          hierarchy,
          entity, 
          component, 
          it->first, 
          it->second, 
          inner);

        inner_layout->addWidget(edit);
        edits_.push_back(edit);
      }
    }

    //--------------------------------------------------------------------------
    void PropertyGroupView::UpdateValues()
    {
      for (int i = 0; i < edits_.size(); ++i)
      {
        edits_.at(i)->Update();
      }
    }
  }
}