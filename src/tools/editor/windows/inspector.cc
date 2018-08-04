#include "tools/editor/windows/inspector.h"
#include "tools/editor/windows/gui.h"

#include <engine/components/transform_component.h>
#include <engine/components/mesh_component.h>
#include <engine/components/mesh_renderer_component.h>
#include <engine/components/camera_component.h>

#ifndef SNUFF_NSCRIPTING
#include <engine/components/script_component.h>
#endif

#include <qlabel.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
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
      remove_component_(nullptr),
      selected_(nullptr)
    {
      ApplyStyle();
      AddContextMenu();
    }

    //--------------------------------------------------------------------------
    void Inspector::ShowEntity(engine::Entity* entity)
    {
      tree_->clear();

      selected_ = entity;

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
      gui.TextField(entity->name().c_str(), [=](QWidget*, const QString& value)
      {
        entity->set_name(value.toStdString().c_str());
      });
      gui.ResetForegroundColor();

      gui.Label("Active");
      gui.Checkbox(entity->active(), [=](QWidget*, bool value)
      {
        entity->set_active(value);
      });

      gui.EndLayout();

      QWidget* top_widget = gui.EndAsWidget();

      gui = GUI();
      
      gui.StartLayout(GUI::LayoutStyle::kVertical);
      gui.SetBackgroundColor(EditorColors::BlueButton());
      gui.Button(
        "Add component", 
        {
          "Script",
          "Mesh",
          "Mesh renderer",
          "Camera"
        }, 
      [=](QWidget* self, int option)
      {
#ifdef SNUFF_NSCRIPTING
        if (option == 0)
        {
          return;
        }
#endif
        entity->AddComponent(static_cast<engine::Components>(option + 1));
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
    engine::Entity* Inspector::selected() const
    {
      return selected_;
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

      LabeledVec("Position", t->GetLocalPosition(),
      [=](QWidget*, int n, float v)
      {
        glm::vec3 p = t->GetLocalPosition();
        p[n] = v;
        t->SetLocalPosition(p);
      });

      LabeledVec("Rotation", t->GetRotationEuler(),
      [=](QWidget*, int n, float v)
      {
        glm::vec3 r = t->GetRotationEuler();
        r[n] = v;
        t->SetRotationEuler(r);
      });

      LabeledVec("Scale", t->GetScale(),
      [=](QWidget*, int n, float v)
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
      parent->setText(0, "Script");

#ifndef SNUFF_NSCRIPTING
      engine::ScriptComponent* s = 
        static_cast<engine::ScriptComponent*>(component);

      GUI gui;

      gui.StartLayout(GUI::LayoutStyle::kVertical);

      gui.Label("Behavior");
      gui.TextField(s->behavior().c_str(),
      [=](QWidget*, const QString& behavior)
      {
        s->SetBehavior(behavior.toStdString().c_str());
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

      engine::MeshRendererComponent* c = 
        static_cast<engine::MeshRendererComponent*>(component);

      GUI gui;

      gui.StartLayout(GUI::LayoutStyle::kVertical);

      gui.Label("Material(s)");

      foundation::Vector<engine::SerializableAsset>& mats = 
        c->shared_materials();

      engine::SerializableAsset* a = nullptr;
      for (size_t i = 0; i < mats.size(); ++i)
      {
        gui.StartLayout(GUI::LayoutStyle::kHorizontal);

        gui.Label(std::string(std::to_string(i + 1) + ". ").c_str());

        a = &mats.at(i);
        gui.AssetField(a, [=](QWidget*, const QString& value)
        {
          engine::Material new_mat = engine::Material(a->handle);
          c->SetMaterial(static_cast<int>(i), &new_mat);
        });

        gui.EndLayout();
      }

      return gui.EndAsWidget();
    }

    //--------------------------------------------------------------------------
    template <>
    inline QWidget* Inspector::ShowComponent<engine::Components::kMesh>(
      engine::IComponent* component,
      QTreeWidgetItem* parent)
    {
      parent->setText(0, "Mesh");

      engine::MeshComponent* c =
        static_cast<engine::MeshComponent*>(component);

      GUI gui;

      gui.StartLayout(GUI::LayoutStyle::kVertical);

      gui.StartLayout(GUI::LayoutStyle::kHorizontal);

      engine::SerializableAsset* a = c->asset();

      gui.Label("Model ");
      gui.AssetField(a, [=](QWidget*, const QString& value)
      {
        engine::Mesh mesh(a->handle, c->scene_index());
        c->SetMesh(&mesh);
      });

      gui.Label(" Index ");
      gui.NumberField(
        static_cast<double>(c->scene_index()), true, [=](QWidget*, double v)
      {
        engine::Mesh mesh(a->handle, static_cast<int>(v));
        c->SetMesh(&mesh);
      });

      gui.EndLayout();

      return gui.EndAsWidget();
    }

    //--------------------------------------------------------------------------
    template <>
    inline QWidget* Inspector::ShowComponent<engine::Components::kCamera>(
      engine::IComponent* component,
      QTreeWidgetItem* parent)
    {
      parent->setText(0, "Camera");

      engine::CameraComponent* c = 
        static_cast<engine::CameraComponent*>(component);

      GUI gui;

      gui.StartLayout(GUI::LayoutStyle::kVertical);

      gui.StartLayout(GUI::LayoutStyle::kHorizontal);

      gui.Label("Near plane ");
      gui.NumberField(static_cast<double>(c->near()), false,
      [=](QWidget*, double val)
      {
        c->set_near(static_cast<float>(val));
      });

      gui.Label(" Far plane ");
      gui.NumberField(static_cast<double>(c->far()), false,
      [=](QWidget*, double val)
      {
        c->set_far(static_cast<float>(val));
      });

      gui.EndLayout();

      gui.StartLayout(GUI::LayoutStyle::kHorizontal);

      gui.Label("Field of view ");
      gui.NumberField(static_cast<double>(c->fov()), false,
        [=](QWidget*, double val)
      {
        c->set_fov(static_cast<float>(val));
      });

      gui.EndLayout();

      gui.StartLayout(GUI::LayoutStyle::kHorizontal);

      gui.Label("Aspect ");
      gui.NumberField(static_cast<double>(c->aspect()), false,
        [=](QWidget*, double val)
      {
        c->set_aspect(static_cast<float>(val));
      });

      gui.EndLayout();

      gui.StartLayout(GUI::LayoutStyle::kHorizontal);

      gui.Label("Orthographic size ");
      gui.NumberField(static_cast<double>(c->orthographic_size()), false,
        [=](QWidget*, double val)
      {
        c->set_orthographic_size(static_cast<float>(val));
      });

      gui.EndLayout();

      gui.StartLayout(GUI::LayoutStyle::kHorizontal);

      gui.Label("Is orthographic");
      gui.Checkbox(
        c->projection() == engine::CameraProjection::kOrthographic,
        [=](QWidget*, bool val)
      {
        c->set_projection(
          val == true ? 
          engine::CameraProjection::kOrthographic : 
          engine::CameraProjection::kPerspective);
      });

      gui.EndLayout();

      return gui.EndAsWidget();
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
        dynamic_cast<InspectorComponentItem*>(tree_->itemAt(p));

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
