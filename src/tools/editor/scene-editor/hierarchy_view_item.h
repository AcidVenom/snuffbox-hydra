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
    *        The UUID in the entity is used for editing operations. As entity
    *        pointers will change during creation/deletion of entities.
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
        HierarchyView* view);

      /**
      * @see HierarchyViewItem::HierarchyViewItem
      *
      * @brief Construct with an optional parent
      *
      * @param[in] parent The parent item of this item
      */
      HierarchyViewItem(
        engine::Entity* entity,
        HierarchyView* view,
        HierarchyViewItem* parent);

      /**
      * @brief Sets the default values of this item
      */
      void SetDefaults();

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
      * @return The contained entity of this item
      */
      engine::Entity* entity() const;

      /**
      * @brief Unmaps this hierarchy view item from the owning hierarchy view
      */
      ~HierarchyViewItem();

    private:

      HierarchyView* view_; //!< The view this item belongs to
      HierarchyViewItem* parent_; //!< The parent of this item

      engine::Entity* entity_; //!< The contained entity of this item
    };
  }
}