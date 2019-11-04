#include "tools/editor/property-editor/property_view.h"

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    PropertyView::PropertyView(HierarchyView* hierarchy, QWidget* parent) :
      QWidget(parent),
      hierarchy_(hierarchy)
    {
      setObjectName(QStringLiteral("PropertyView"));
      setMinimumSize(200, 100);

      setBackgroundRole(QPalette::ColorRole::Base);
      setAutoFillBackground(true);
    }
  }
}