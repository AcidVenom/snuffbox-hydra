#include "tools/editor/windows/inspector.h"
#include "tools/editor/windows/gui.h"

#include <engine/components/transform_component.h>

#ifndef SNUFF_NSCRIPTING
#include <engine/components/script_component.h>
#endif

#include <qlabel.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qmenu.h>
#include <qaction.h>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    InspectorComponentItem::InspectorComponentItem(
      engine::Components type,
      engine::IComponent* component)
      :
      type_(type),
      component_(component)
    {

    }

    //--------------------------------------------------------------------------
    engine::Components InspectorComponentItem::type() const
    {
      return type_;
    }

    //--------------------------------------------------------------------------
    engine::IComponent* InspectorComponentItem::component() const
    {
      return component_;
    }

    //--------------------------------------------------------------------------
    Inspector::Inspector(QTreeWidget* widget) :
      tree_(widget),
      context_menu_(nullptr),
      remove_component_(nullptr)
    {
      ApplyStyle();
      AddContextMenu();
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
      });
      gui.ResetForegroundColor();

      gui.Label("Active");
      gui.Checkbox(entity->active(), [=](bool value)
      {
        entity->set_active(value);
      });

      gui.EndLayout();

      QWidget* top_widget = gui.EndAsWidget();

      gui = GUI();
      
      gui.StartLayout(GUI::LayoutStyle::kVertical);
      gui.SetBackgroundColor(EditorColors::BlueButton());
      gui.Button("Add component", [=]()
      {
#ifndef SNUFF_NSCRIPTING
        entity->AddComponent<engine::ScriptComponent>();
#endif
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

      auto LabeledVec = [&](
        const char* label, 
        const glm::vec3& value,
        GUI::ChangeCallback<int, float> on_changed)
      {
        gui.Label(label);
        gui.VectorField<3>(value, on_changed);
      };

      gui.StartLayout(GUI::LayoutStyle::kVertical);

      LabeledVec("Position", t->GetLocalPosition(), [=](int n, float v)
      {
        glm::vec3 p = t->GetLocalPosition();
        p[n] = v;
        t->SetLocalPosition(p);
      });

      LabeledVec("Rotation", t->GetRotationEuler(), [=](int n, float v)
      {
        glm::vec3 r = t->GetRotationEuler();
        r[n] = v;
        t->SetRotationEuler(r);
      });

      LabeledVec("Scale", t->GetScale(), [=](int n, float v)
      {
        glm::vec3 s = t->GetScale();
        s[n] = v;
        t->SetScale(s);
      });

      return gui.EndAsWidget();
    }

    //--------------------------------------------------------------------------
    template <>
    inline QWidget* Inspector::ShowComponent<engine::Components::kScript>(
      engine::IComponent* component,
      QTreeWidgetItem* parent)
    {
#ifndef SNUFF_NSCRIPTING
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
        SetName();
      });

      return gui.EndAsWidget();
#else
      return nullptr;
#endif
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
      tree_->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
      tree_->setSelectionMode(QAbstractItemView::NoSelection);
      tree_->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    }

    //--------------------------------------------------------------------------
    void Inspector::AddContextMenu()
    {
      context_menu_ = new QMenu();

      remove_component_ = new QAction("Remove component", context_menu_);
      context_menu_->addAction(remove_component_);

      connect(
        tree_, 
        &QTreeWidget::customContextMenuRequested,
        this,
        &Inspector::OnCustomContextMenu);
    }

    //--------------------------------------------------------------------------
    void Inspector::OnCustomContextMenu(const QPoint& p)
    {
      QModelIndex index = tree_->indexAt(p);
      remove_component_->setEnabled(index.isValid());

      InspectorComponentItem* item = 
        static_cast<InspectorComponentItem*>(tree_->itemAt(p));

      engine::IComponent* component = nullptr;

      if (
        item == nullptr || 
        (component = item->component()) == nullptr ||
        item->type() == engine::Components::kTransform)
      {
        return;
      }

      QAction* action = context_menu_->exec(tree_->viewport()->mapToGlobal(p));

      if (action == remove_component_)
      {
        engine::Entity* e = component->entity();
        e->RemoveComponentByRef(component);
        ShowEntity(e);
      }
    }

    //--------------------------------------------------------------------------
    Inspector::~Inspector()
    {
      if (context_menu_ != nullptr)
      {
        delete context_menu_;
        context_menu_ = nullptr;
      }
    }
  }
}
