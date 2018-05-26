#pragma once

#include <qobject.h>

class QTreeWidget;

namespace snuffbox
{
  namespace engine
  {
    class Entity;
  }

  namespace editor
  {
    /**
    * @brief The inspector window to inspect entities and their components with
    *
    * The inspector will also show the custom editor fields that can be
    * edited to adjust an entity's properties.
    *
    * @author Daniel Konings
    */
    class Inspector : public QObject
    {

      Q_OBJECT

    public:

      /**
      * @brief Construct the inspector wrapper through its main widget
      *
      * @param[in] widget The main tree widget this inspector will operate on
      */
      Inspector(QTreeWidget* widget);

      /**
      * @brief Shows the data contained within an entity, including
      *        its components
      *
      * @param[in] entity The entity to show in the inspector
      */
      void ShowEntity(engine::Entity* entity);

    signals:

      /**
      * @brief Used to refresh the hierarchy
      */
      void RefreshHierarchy();

    private:

      QTreeWidget* tree_; //!< The tree to append inspector fields to
    };
  }
}