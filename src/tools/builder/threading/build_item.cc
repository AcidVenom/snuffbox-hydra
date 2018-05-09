#include "tools/builder/threading/build_item.h"

namespace snuffbox
{
  namespace builder
  {
    //--------------------------------------------------------------------------
    bool BuildItem::Compare::operator()(
      const BuildItem& a,
      const BuildItem& b)
    {
      return a.type < b.type;
    }
  }
}