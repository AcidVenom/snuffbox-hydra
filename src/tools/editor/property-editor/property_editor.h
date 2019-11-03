#pragma once

#include "tools/editor/property-editor/property_mappings.h"
#include "tools/editor/property-editor/property_view.h"

class QWidget;

namespace snuffbox
{
  namespace editor
  {
    class PropertyEditorBase
    {

    public:

      virtual void Update() = 0;
    };

    template <typename T>
    class PropertyEditor : public PropertyEditorBase
    {
      using Mapping = PropertyMapping<T>;

    public:

      void Show(T* object, QWidget* parent);

      void Update() override;

    private:

      using ViewMap = foundation::Map<foundation::String, foundation::SharedPtr<PropertyView>>;

      T* object_;
      ViewMap views_;
    };

    template <typename T>
    void PropertyEditor<T>::Show(T* object, QWidget* parent)
    {
      object_ = object;
      Update();
    }

    template <typename T>
    void PropertyEditor<T>::Update()
    {
      PropertyMap map = Mapping::kMapping;
      for (PropertyMap::iterator it = map.begin(); it != map.end(); ++it)
      {
        foundation::String prop_name = it->first;
        ViewMap::iterator view = views_.find(prop_name);

        if (view == views_.end())
        {
          foundation::SharedPtr<PropertyView> view =
            foundation::Memory::ConstructShared<PropertyView>(
              &foundation::Memory::default_allocator(),
              object_,
              it->second);

          view = views_.insert(
            eastl::make_pair<
            foundation::String, 
            foundation::SharedPtr<PropertyView>
            >(prop_name, view));
        }

        view->second->Update(object_);
      }
    }
  }
}