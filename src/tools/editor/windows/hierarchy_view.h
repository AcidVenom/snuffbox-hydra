#pragma once

#include <qobject.h>

class QTreeWidget;
class QMenu;
class QAction;

namespace snuffbox
{
  namespace editor
  {
    class HierarchyView : public QObject
    {

      Q_OBJECT

    public:

      /**
      * @brief Construct a hierarchy view wrapper around a tree view
      */
      HierarchyView(QTreeWidget* widget);

    protected:

      /**
      * @brief Adds the context menu to the hierarchy view
      */
      void AddContextMenu();

    private slots:

      void OnCustomContextMenu(const QPoint& p);

    private:

      QTreeWidget* tree_; //!< The underlying Qt tree widget
      QMenu* context_menu_; //!< The context menu
      QAction* add_entity_; //!< The add entity option
      QAction* remove_entity_; //!< The remove entity option
    };
  }
}