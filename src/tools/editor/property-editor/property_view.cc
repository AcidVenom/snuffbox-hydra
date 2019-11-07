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
      setObjectName(QStringLiteral("PropertyView"));
      setMinimumSize(200, 100);

      setBackgroundRole(QPalette::ColorRole::Base);
      setAutoFillBackground(true);

      layout_ = new QVBoxLayout();
      layout_->setMargin(5);

      setLayout(layout_);

      /*QScrollArea* scroll_area = new QScrollArea();
      scroll_area->setObjectName(QStringLiteral("PropertyViewScrollArea"));
      scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
      scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
      scroll_area->setWidgetResizable(true);
      scroll_area->setWidget(this);

      engine::Entity* ent = foundation::Memory::Construct<engine::Entity>(&foundation::Memory::default_allocator());
      ShowForEntity(ent);*/
    }

    //--------------------------------------------------------------------------
    void PropertyView::ShowForEntity(engine::Entity* ent)
    {
      Clear();

      const PropertyMap& map = PropertyMappings::GetEntityMap();

      for (
        PropertyMap::const_iterator it = map.begin();
        it != map.end();
        ++it)
      {
        PropertyValueEdit* edit = 
          new PropertyValueEdit(this, ent, it->first, it->second, this);

        items_.push_back(edit);

        layout_->addWidget(edit);
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