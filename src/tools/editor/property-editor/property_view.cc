#include "tools/editor/property-editor/property_view.h"

#include "tools/editor/property-editor/property_value_edit.h"
#include "tools/editor/property-editor/property_mappings.h"
#include "tools/editor/property-editor/property_group_view.h"

#include <QVBoxLayout>
#include <QScrollArea>
#include <QLabel>

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
      layout_(nullptr)
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

      const PropertyMap& entity_map = PropertyMappings::GetEntityMap();

      PropertyGroupView* view = 
        new PropertyGroupView(QStringLiteral("Entity"), entity_map, ent);

      layout_->addWidget(view);
      views_.push_back(view);

      QString header_names[static_cast<int>(engine::Components::kCount)];
      
      auto DefineName = [&header_names](engine::Components c, const char* name)
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

        for (int i = 0; i < components.size(); ++i)
        {
          engine::IComponent* component = components.at(i);
          view = 
            new PropertyGroupView(header_names[i], component_map, component);

          layout_->addWidget(view);
          views_.push_back(view);
        }
      }
    }

    //--------------------------------------------------------------------------
    void PropertyView::Refresh()
    {

    }

    //--------------------------------------------------------------------------
    void PropertyView::Clear()
    {
      for (int i = 0; i < views_.size(); ++i)
      {
        delete views_.at(i);
      }

      views_.clear();
    }
  }
}