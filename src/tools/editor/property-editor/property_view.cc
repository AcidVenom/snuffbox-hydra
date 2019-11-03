#include "tools/editor/property-editor/property_view.h"
#include "tools/editor/property-editor/property_value.h"

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    PropertyView::PropertyView(
      foundation::SharedPtr<PropertyValue> prop, 
      QWidget* parent) :
      QWidget(parent),
      property_(prop)
    {

    }

    //--------------------------------------------------------------------------
    void PropertyView::Update(void* object)
    {
      if (property_ == nullptr)
      {
        return;
      }
    }
  }
}