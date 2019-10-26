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
      */
      HierarchyViewItem(engine::Entity* entity, HierarchyView* view);

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

    private:

      HierarchyView* view_; //!< The view this item belongs to
      HierarchyViewItem* parent_; //!< The parent of this item

      engine::Entity* entity_; //!< The contained entity of this item
    };
  }
}