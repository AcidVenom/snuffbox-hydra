#include "tools/editor/windows/hierarchy_view.h"

#include <qmenu.h>
#include <qaction.h>
#include <qtreewidget.h>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    HierarchyView::HierarchyView(QTreeWidget* widget) :
      tree_(widget)
    {
      AddContextMenu();
    }

    //--------------------------------------------------------------------------
    void HierarchyView::AddContextMenu()
    {
      context_menu_ = new QMenu();
      tree_->setContextMenuPolicy(Qt::CustomContextMenu);

      add_entity_ = new QAction("Add Entity", context_menu_);
      context_menu_->addAction(add_entity_);

      remove_entity_ = new QAction("Remove Entity", context_menu_);
      context_menu_->addAction(remove_entity_);

      connect(
        tree_,
        &QTreeWidget::customContextMenuRequested,
        this,
        &HierarchyView::OnCustomContextMenu);
    }

    //--------------------------------------------------------------------------
    void HierarchyView::OnCustomContextMenu(const QPoint& p)
    {
      QModelIndex index = tree_->indexAt(p);
      remove_entity_->setEnabled(index.isValid());

      context_menu_->exec(tree_->viewport()->mapToGlobal(p));
    }
  }
}