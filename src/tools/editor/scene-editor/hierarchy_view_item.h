#pragma once

#include <QTreeWidget>

namespace snuffbox
{
  namespace engine
  {
    class Entity;
  }

  namespace editor
  {
    class HierarchyView;

    /**
    * @brief Used to contain a single entity and update to its properties
    *        accordingly. This also allows for renaming and deletion of items,
    *        along with re-parenting.
    *
    *        Alongside of storing entity data, an index is also stored to
    *        retrieve entities by. The format of this index is a string,
    *        specifying how to index the tree.
    *        This functionality is in place because we cannot guarantee an 
    *        entity's pointer being the same after redo or undo. 
    *        This way, we can look up entities with their index as if a user
    *        would expand tabs normally.
    *
    * @author Daniel Konings
    */
    class HierarchyViewItem : public QTreeWidgetItem
    {

    public:

      /**
      * @brief Construct through the working entity and a hierarchy view
      *
      * @param[in] entity The contained entity of this item
      * @param[in] view The view this item belongs to
      * @param[in] index A string representing how to index this item
      */
      HierarchyViewItem(
        engine::Entity* entity, 
        HierarchyView* view,
        const QString& index);

      /**
      * @brief Sets the parent of this item
      *
      * @param[in] new_parent The parent to set
      *
      * @return Was the parent actually changed?
      */
      bool SetParent(HierarchyViewItem* new_parent);

      /**
      * @brief Updates this item, setting the name, and whether it is active
      *        or not
      */
      void Update();

      /**
      * @brief Updates the indexing retrieval string of this item
      *
      * @param[in] index The new value
      */
      void UpdateIndex(const QString& index);

      /**
      * @return The contained entity of this item
      */
      engine::Entity* entity() const;

      /**
      * @return The index associated with this entity
      */
      const QString& index() const;

    private:

      HierarchyView* view_; //!< The view this item belongs to
      QString index_; //!< The index of this item
      HierarchyViewItem* parent_; //!< The parent of this item

      engine::Entity* entity_; //!< The contained entity of this item
    };
  }
}