#include "tools/editor/windows/asset_browser.h"

#include <qfilesystemmodel.h>
#include <qtreeview.h>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    AssetBrowser::AssetBrowser(QTreeView* tree, QLayout* assets) :
      tree_(tree),
      assets_(assets),
      model_(nullptr)
    {
      
    }

    //--------------------------------------------------------------------------
    void AssetBrowser::Refresh(const foundation::Path& path)
    {
      if (model_ != nullptr)
      {
        delete model_;
        model_ = nullptr;
      }

      model_ = new QFileSystemModel(this);

      model_->setReadOnly(true);
      model_->setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

      QString root = path.ToString().c_str();
      model_->setRootPath(root);

      tree_->setModel(model_);
      tree_->setRootIndex(model_->index(root));
    }
  }
}