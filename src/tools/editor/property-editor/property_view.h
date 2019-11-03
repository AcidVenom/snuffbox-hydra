#pragma once

#include <foundation/memory/memory.h>

#include <QWidget>

namespace snuffbox
{
  namespace editor
  {
    class PropertyValue;

    class PropertyView : public QWidget
    {

      Q_OBJECT;

    public:

      PropertyView(foundation::SharedPtr<PropertyValue> prop, QWidget* parent);

      void Update(void* object);

    private:

      foundation::SharedPtr<PropertyValue> property_;
    };
  }
}