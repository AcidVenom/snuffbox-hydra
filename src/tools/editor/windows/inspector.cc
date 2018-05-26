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

      const QPalette& palette = EditorColors::DefaultPalette();

      QTreeWidgetItem* top = new QTreeWidgetItem();
      top->setBackgroundColor(0, EditorColors::DockColor());

      GUI gui;
      gui.StartLayout(GUI::LayoutStyle::kVertical);

      gui.StartLayout(GUI::LayoutStyle::kHorizontal);

      gui.SetSpacing(10);

      gui.Label("Name");
      gui.TextField(entity->name().c_str(), [=](const QString& value)
      {
        entity->set_name(value.toStdString().c_str());
        emit RefreshHierarchy();
      });
      gui.ResetForegroundColor();

      gui.Label("Active");
      gui.Checkbox(entity->active(), [=](bool value)
      {
        entity->set_active(value);
        emit RefreshHierarchy();
      });

      gui.EndLayout();

      QWidget* top_widget = gui.EndAsWidget();

      gui = GUI();
      gui.StartLayout(GUI::LayoutStyle::kVertical);
      gui.SetBackgroundColor(palette.color(QPalette::ColorRole::Button));
      gui.Button("Add component");
      gui.ResetBackgroundColor();
      
      QWidget* comp_widget = gui.EndAsWidget();

      tree_->addTopLevelItem(top);
      tree_->setItemWidget(top, 0, top_widget);

      QTreeWidgetItem* components = new QTreeWidgetItem();
      top->addChild(components);
      tree_->setItemWidget(components, 0, comp_widget);
    }
  }
}