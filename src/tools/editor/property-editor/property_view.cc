#include "tools/editor/property-editor/property_view.h"

#include "tools/editor/property-editor/property_value_edit.h"
#include "tools/editor/property-editor/property_mappings.h"
#include "tools/editor/property-editor/property_group_view.h"

#include "tools/editor/application/styling.h"
#include "tools/editor/editor-widgets/searcher_widget.h"

#include "tools/editor/scene-editor/entity_commands.h"
#include "tools/editor/scene-editor/hierarchy_view.h"

#include <QVBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>

#include <engine/ecs/entity.h>


namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    PropertyView::PropertyView(HierarchyView* hierarchy, QWidget* parent) :
      QWidget(parent),
      hierarchy_(hierarchy),
      frame_(nullptr),
      layout_(nullptr),
      add_component_(nullptr),
      entity_(nullptr),
      old_component_count_(0)
    {
      int min_width = 200;

      setObjectName(QStringLiteral("PropertyView"));
      setMinimumSize(min_width, 100);

      QVBoxLayout* main_layout = new QVBoxLayout(this);
      main_layout->setContentsMargins(0, 0, 0, 0);
      main_layout->setSpacing(0);

      frame_ = new QWidget(this);
      layout_ = new QVBoxLayout(frame_);
      layout_->setContentsMargins(0, 0, 0, 0);
      layout_->setSpacing(0);

      QScrollArea* scroll_area = new QScrollArea(this);
      scroll_area->setObjectName(QStringLiteral("PropertyViewScrollArea"));
      scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
      scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
      scroll_area->setWidgetResizable(true);
      scroll_area->setWidget(frame_);

      frame_->setLayout(layout_);
      frame_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

      main_layout->addWidget(scroll_area);

      setAutoFillBackground(true);
      setBackgroundRole(QPalette::ColorRole::Base);

      setLayout(main_layout);
      setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    //--------------------------------------------------------------------------
    void PropertyView::ShowForEntity(engine::Entity* ent)
    {
      Clear();

      if (ent == nullptr)
      {
        return;
      }

      entity_ = ent;
      old_component_count_ = entity_->ComponentCount();

      const PropertyMap& entity_map = PropertyMappings::GetEntityMap();

      PropertyGroupView* view = new PropertyGroupView(
        hierarchy_,
        QStringLiteral("Entity"), 
        entity_map, 
        ent, 
        nullptr,
        engine::Components::kCount,
        -1,
        this);

      layout_->addWidget(view);
      views_.push_back(view);

      QString header_names[static_cast<int>(engine::Components::kCount)];
      
      auto DefineName = [&](engine::Components c, const char* name)
      {
        header_names[static_cast<int>(c)] = QString(name);
      };

      using Comp = engine::Components;

      DefineName(Comp::kTransform, "Transform Component");
      DefineName(Comp::kScript, "Script Component");
      DefineName(Comp::kMesh, "Mesh Component");
      DefineName(Comp::kMeshRenderer, "Mesh Renderer Component");
      DefineName(Comp::kCamera, "Camera Component");

      for (
        engine::Components c = engine::Components::kTransform;
        c != engine::Components::kCount;
        c = static_cast<engine::Components>(static_cast<int>(c) + 1))
      {
        const PropertyMap& component_map = PropertyMappings::GetComponentMap(c);
        
        foundation::Vector<engine::IComponent*> components = 
          ent->GetComponents(c);

        const QString& header_name = header_names[static_cast<int>(c)];
        for (int i = 0; i < components.size(); ++i)
        {
          engine::IComponent* component = components.at(i);
          view = new PropertyGroupView(
            hierarchy_,
            header_name,
            component_map, 
            ent, 
            component,
            c,
            i,
            this);

          layout_->addWidget(view);
          views_.push_back(view);
        }
      }

      add_component_ = new QPushButton(this);
      add_component_->setObjectName(QStringLiteral("AddComponentButton"));
      add_component_->setText(QStringLiteral("Add component"));

      QString col = Styling::GetStyleColorCSS(Styling::ColorRole::kBlueButton);
      add_component_->setStyleSheet("background: " + col);

      connect(add_component_, &QPushButton::released, this, [=]()
      {
        SearcherWidget* searcher = new SearcherWidget();
        for (engine::Components c = engine::Components::kScript;
          c != engine::Components::kCount;
          c = static_cast<engine::Components>(static_cast<int>(c) + 1))
        {
          searcher->AddItem(header_names[static_cast<int>(c)]);
        }

        connect(
          searcher, 
          &SearcherWidget::Selected, 
          this, 
          &PropertyView::OnAddComponent);

        searcher->Show();
      });

      layout_->addWidget(add_component_);
    }

    //--------------------------------------------------------------------------
    void PropertyView::Update()
    {
      if (entity_ == nullptr)
      {
        return;
      }

      if (entity_->ComponentCount() != old_component_count_)
      {
        ShowForEntity(entity_);
        return;
      }

      for (int i = 0; i < views_.size(); ++i)
      {
        views_.at(i)->UpdateValues();
      }
    }

    //--------------------------------------------------------------------------
    void PropertyView::Clear()
    {
      for (int i = 0; i < views_.size(); ++i)
      {
        delete views_.at(i);
      }

      views_.clear();

      delete add_component_;
      add_component_ = nullptr;
    }

    //--------------------------------------------------------------------------
    void PropertyView::OnAddComponent(int index)
    {
      engine::Components id = static_cast<engine::Components>(index + 1);
      
      if (entity_ == nullptr)
      {
        return;
      }

      AddComponentEntityCommand* cmd = new AddComponentEntityCommand(
        entity_->uuid(),
        hierarchy_,
        id);

      hierarchy_->PushUndoCommand(cmd);
    }
  }
}