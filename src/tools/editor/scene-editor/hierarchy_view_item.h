#pragma once

#include <QTreeWidget>
#include <QUuid>

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
    *        Alongside of storing entity data, a UUID is also stored to easily
    *        retrieve entities by UUID. This functionality is in place because
    *        we cannot guarantee an entity's pointer being the same after
    *        redo or undo. This way, we can look up entities with their global
    *        UUID. This is not implemented in the runtime of the engine itself,
    *        as it is not deemed necessary there.
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
      * @param[in] uuid The UUID to store for this item
      */
      HierarchyViewItem(
        engine::Entity* entity, 
        HierarchyView* view,
        const QUuid& uuid);

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
      * @brief Updates the UUID of this item
      *
      * @param[in] uuid The new UUID
      */
      void UpdateUUID(const QUuid& uuid);

      /**
      * @return The contained entity of this item
      */
      engine::Entity* entity() const;

      /**
      * @return The UUID associated with this entity
      */
      const QUuid& uuid() const;

    private:

      HierarchyView* view_; //!< The view this item belongs to
      HierarchyViewItem* parent_; //!< The parent of this item

      engine::Entity* entity_; //!< The contained entity of this item
      QUuid uuid_; //!< The UUID associated with this entity
    };
  }
}