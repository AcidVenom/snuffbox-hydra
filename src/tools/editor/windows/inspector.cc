#include "tools/editor/windows/inspector.h"
#include "tools/editor/windows/gui.h"
#include "tools/editor/definitions/editor_colors.h"

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

      GUI gui;
      gui.StartLayout(GUI::LayoutStyle::kVertical);

      gui.StartLayout(GUI::LayoutStyle::kHorizontal);

      gui.SetSpacing(10);

      gui.Label("Name: ");
      gui.TextField(entity->name().c_str());
      gui.ResetForegroundColor();

      gui.Label("Active: ");
      gui.Checkbox(entity->active());

      gui.EndLayout();

      QLayout* layout = gui.EndLayout();

      QTreeWidgetItem* top = new QTreeWidgetItem();
      top->setBackgroundColor(0, EditorColors::DockColor());

      QWidget* widget = new QWidget();
      widget->setLayout(layout);

      tree_->addTopLevelItem(top);
      tree_->setItemWidget(top, 0, widget);
    }
  }
}