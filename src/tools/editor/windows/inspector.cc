#include "tools/editor/windows/inspector.h"

#include <engine/ecs/entity.h>

#include <qtreewidget.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qlayout.h>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    Inspector::Inspector(QTreeWidget* widget) :
      tree_(widget)
    {
      
    }

    //--------------------------------------------------------------------------
    void Inspector::ShowEntity(engine::Entity* entity)
    {
      tree_->clear();

      if (entity == nullptr)
      {
        return;
      }

      QTreeWidgetItem* item = new QTreeWidgetItem();

      QWidget* widget = new QWidget();
      QHBoxLayout* layout = new QHBoxLayout();

      QLabel* label_name = new QLabel();
      label_name->setText("Name: ");

      QLineEdit* name_field = new QLineEdit();
      name_field->setText(entity->name().c_str());

      QLabel* lable_active = new QLabel();
      lable_active->setText("Active: ");

      QCheckBox* is_active_field = new QCheckBox();
      is_active_field->setChecked(entity->active());

      layout->addWidget(label_name);
      layout->addWidget(name_field);
      layout->addWidget(lable_active);
      layout->addWidget(is_active_field);
      layout->addStretch();

      layout->setSizeConstraint(QLayout::SetFixedSize);
      widget->setLayout(layout);

      item->setSizeHint(0, widget->sizeHint());
      item->setBackgroundColor(0, QColor(60, 66, 57));

      QTreeWidgetItem* top = new QTreeWidgetItem();
      top->setText(0, "Entity");
      top->addChild(item);
      top->setSizeHint(0, QSize(0, 24));
      top->setBackgroundColor(0, QColor(110, 126, 103));
      tree_->setItemWidget(item, 0, widget);

      tree_->addTopLevelItem(top);
    }
  }
}