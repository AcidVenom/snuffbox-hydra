#include "tools/editor/asset-browser/asset_browser_item.h"

#include "tools/editor/asset-browser/asset_icon.h"

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    const QSize AssetBrowserItem::kMaxItemSize = QSize(128, 96);

    //--------------------------------------------------------------------------
    AssetBrowserItem::AssetBrowserItem(
      compilers::AssetTypes type,
      const QString& path,
      QWidget* parent)
      :
      QWidget(parent),
      type_(type),
      path_(path),
      icon_(nullptr)
    {
      setObjectName(QStringLiteral("AssetBrowserItem"));

      icon_ = new AssetIcon(type, path, this);

      setMinimumSize(kMaxItemSize);
      setMaximumSize(kMaxItemSize);
    }
  }
}