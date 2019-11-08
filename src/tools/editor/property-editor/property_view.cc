#include "tools/editor/property-editor/property_view.h"

#include "tools/editor/property-editor/property_value_edit.h"
#include "tools/editor/property-editor/property_mappings.h"

#include <QVBoxLayout>
#include <QScrollArea>

#include <engine/ecs/entity.h>


namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    PropertyView::PropertyView(HierarchyView* hierarchy, QWidget* parent) :
      QWidget(parent),
      hierarchy_(hierarchy),
      layout_(nullptr)
    {
      int min_width = 200;

      setObjectName(QStringLiteral("PropertyView"));
      setMinimumSize(min_width, 100);

      QVBoxLayout* main_layout = new QVBoxLayout(this);
      main_layout->setContentsMargins(0, 0, 0, 0);
      main_layout->setSpacing(0);

      QWidget* frame = new QWidget(this);
      layout_ = new QVBoxLayout(frame);
      layout_->setContentsMargins(0, 0, 0, 0);
      layout_->setSpacing(2);

      QScrollArea* scroll_area = new QScrollArea(this);
      scroll_area->setObjectName(QStringLiteral("PropertyViewScrollArea"));
      scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
      scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
      scroll_area->setWidgetResizable(true);
      scroll_area->setWidget(frame);

      frame->setLayout(layout_);
      frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

      main_layout->addWidget(scroll_area);

      setAutoFillBackground(true);
      setBackgroundRole(QPalette::ColorRole::Base);

      setLayout(main_layout);
    }

    //--------------------------------------------------------------------------
    void PropertyView::ShowForEntity(engine::Entity* ent)
    {
      Clear();

      auto AddElements = [&](const PropertyMap& mapping, void* object)
      {
        for (
          PropertyMap::const_iterator it = mapping.begin();
          it != mapping.end();
          ++it)
        {
          PropertyValueEdit* edit = 
            new PropertyValueEdit(this, object, it->first, it->second, this);

          items_.push_back(edit);

          layout_->addWidget(edit, 0, Qt::AlignTop);
        }
      };

      const PropertyMap& entity_map = PropertyMappings::GetEntityMap();

      AddElements(entity_map, ent);

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
          AddElements(component_map, component);
        }
      }
    }

    //--------------------------------------------------------------------------
    void PropertyView::Update()
    {

    }

    //--------------------------------------------------------------------------
    void PropertyView::Clear()
    {
      for (int i = 0; i < items_.size(); ++i)
      {
        delete items_.at(i);
      }

      items_.clear();
    }
  }
}