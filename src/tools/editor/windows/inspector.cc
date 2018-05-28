#include "tools/editor/windows/inspector.h"
#include "tools/editor/windows/gui.h"
#include "tools/editor/definitions/editor_colors.h"

#include <engine/components/transform_component.h>
#include <engine/components/script_component.h>

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
      refreshing_(false),
      tree_(widget)
    {
      ApplyStyle();
    }

    //--------------------------------------------------------------------------
    void Inspector::ShowEntity(engine::Entity* entity)
    {
      refreshing_ = true;
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
      gui.SetBackgroundColor(EditorColors::BlueButton());
      gui.Button("Add component", [=]()
      {
        entity->AddComponent<engine::ScriptComponent>();
        ShowEntity(entity);
      });

      gui.ResetBackgroundColor();
      
      QWidget* add_comp = gui.EndAsWidget();

      tree_->addTopLevelItem(top);
      tree_->setItemWidget(top, 0, top_widget);

      QTreeWidgetItem* comp = new QTreeWidgetItem();
      tree_->addTopLevelItem(comp);
      tree_->setItemWidget(comp, 0, add_comp);

      ShowComponents(entity, top);
      top->setExpanded(true);
      refreshing_ = false;
    }

    //--------------------------------------------------------------------------
    template <>
    inline QWidget* Inspector::ShowComponent<engine::Components::kTransform>(
      engine::IComponent* component,
      QTreeWidgetItem* parent)
    {
      parent->setText(0, "Transform");

      engine::TransformComponent* t = 
        static_cast<engine::TransformComponent*>(component);

      GUI gui;

      auto LabeledVec = [&](const char* label)
      {
        gui.Label(label);
        gui.VectorField<3>({0.0f, 0.0f, 0.0f});
      };

      gui.StartLayout(GUI::LayoutStyle::kVertical);

      LabeledVec("Position");
      LabeledVec("Rotation");
      LabeledVec("Scale");

      return gui.EndAsWidget();
    }

    //--------------------------------------------------------------------------
    template <>
    inline QWidget* Inspector::ShowComponent<engine::Components::kScript>(
      engine::IComponent* component,
      QTreeWidgetItem* parent)
    {
      engine::ScriptComponent* s = 
        static_cast<engine::ScriptComponent*>(component);

      auto SetName = [=]()
      {
        parent->setText(0, QString("Script (") + s->behavior().c_str() + ")");
      };

      SetName();

      GUI gui;

      gui.StartLayout(GUI::LayoutStyle::kVertical);

      gui.Label("Behavior");
      gui.TextField(s->behavior().c_str(), [=](const QString& behavior)
      {
        s->SetBehavior(behavior.toStdString().c_str());

        if (refreshing_ == false)
        {
          SetName();
        }
      });

      return gui.EndAsWidget();
    }

    //--------------------------------------------------------------------------
    template <>
    inline QWidget* Inspector::ShowComponent<engine::Components::kMeshRenderer>(
      engine::IComponent* component,
      QTreeWidgetItem* parent)
    {
      parent->setText(0, "Mesh Renderer");
      return nullptr;
    }

    //--------------------------------------------------------------------------
    void Inspector::ApplyStyle()
    {
      tree_->setIndentation(0);
      tree_->setSelectionMode(QAbstractItemView::NoSelection);
      tree_->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    }
  }
}