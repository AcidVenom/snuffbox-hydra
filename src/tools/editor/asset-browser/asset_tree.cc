#include "tools/editor/asset-browser/asset_tree.h"

#include <foundation/io/directory_tree.h>
#include <foundation/containers/function.h>
#include <foundation/auxiliary/logger.h>

#include <QFileSystemModel>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    AssetTree::AssetTree(const QString& build_dir, QWidget* parent) :
      QTreeView(parent),
      build_dir_(build_dir),
      model_(nullptr)
    {
      setObjectName(QStringLiteral("AssetTree"));

      setWindowTitle(QStringLiteral("Directory navigator"));

      foundation::Path base_dir = build_dir_.toLatin1().data();
      base_dir = base_dir.GetBaseDirectory();

      QString q_base_dir = base_dir.ToString().c_str();

      model_ = new QFileSystemModel();
      model_->setRootPath(q_base_dir);
      model_->setReadOnly(true);
      model_->setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

      setModel(model_);
      setRootIndex(model_->index(q_base_dir));
      setAnimated(true);
    }

    //--------------------------------------------------------------------------
    void AssetTree::selectionChanged(
      const QItemSelection& selected, 
      const QItemSelection& deselected)
    {
      QTreeView::selectionChanged(selected, deselected);

      if (selected.count() <= 0)
      {
        return;
      }

      QModelIndexList indices = selectionModel()->selectedIndexes();

      if (indices.size() == 0)
      {
        return;
      }

      QString directory = model_->filePath(indices.first());
      emit DirectorySelected(directory);
    }

    //--------------------------------------------------------------------------
    AssetTree::~AssetTree()
    {
      if (model_ != nullptr)
      {
        delete model_;
        model_ = nullptr;
      }
    }
  }
}