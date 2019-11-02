#pragma once

#include <foundation/containers/uuid.h>

#include <QUndoCommand>
#include <QModelIndex>

namespace snuffbox
{
  namespace editor
  {
    class HierarchyView;
    class HierarchyViewItem;

    /**
    * @brief The base class of every command that does something with entities.
    *        This class guarantees that we have a reference to the hierarchy
    *        view and that we have a valid index to operate with.
    *
    * @author Daniel Konings
    */
    class EntityCommand : public QUndoCommand
    {

    protected:

      /**
      * @brief No default constructor
      */
      EntityCommand() = delete;

      /**
      * @brief Construct by UUID and the hierarchy view this command got
      *        created from
      *
      * @param[in] uuid The UUID of the entity to operate on
      * @param[in] view The hierarchy view that invoked this command
      */
      EntityCommand(const foundation::UUID& uuid, HierarchyView* view);

      /**
      * @return The hierarchy view that invoked this command
      */
      HierarchyView* view() const;

      /**
      * @return The UUID of the entity to operate on
      */
      const foundation::UUID& uuid() const;

      /**
      * @brief Called when this command is executed initially or redone
      */
      void redo() override;

      /**
      * @see EntityCommand::redo
      */
      virtual void RedoImpl() = 0;

      /**
      * @brief Called after a redo, after the scene changed callback is 
      *        unlocked
      */
      virtual void PostRedo();

      /**
      * @brief Called when this entity command is undone
      */
      void undo() override;

      /**
      * @see EntityCommand::undo
      */
      virtual void UndoImpl() = 0;

      /**
      * @brief Called after an undo, after the scene changed callback is 
      *        unlocked
      */
      virtual void PostUndo();

      /**
      * @brief Retrieves the hierarchy view item by a UUID
      *
      * @param[in] uuid The UUID to retrieve
      *
      * @return The found item, or nullptr if it doesn't exist, or the UUID
      *         was not valid
      */
      HierarchyViewItem* GetFromUUID(const foundation::UUID& uuid) const;

      /**
      * @return The entity this entity command operates on
      */
      HierarchyViewItem* GetSelf() const;

    private:

      foundation::UUID uuid_; //!< The UUID of the entity to operate on
      HierarchyView* view_; //!< The hierarchy view that invoked this command
    };
    //--------------------------------------------------------------------------

    /**
    * @brief A command to create entities, or delete them when undone
    *
    * @author Daniel Konings
    */
    class CreateEntityCommand : public EntityCommand
    {

    public:
      
      /**
      * @see EntityCommand::EntityCommand
      */
      CreateEntityCommand(const foundation::UUID& uuid, HierarchyView* view);

      /**
      * @see EntityCommand::RedoImpl
      */
      void RedoImpl() override;

      /**
      * @see EntityCommand::UndoImpl
      */
      void UndoImpl() override;
    };
    //--------------------------------------------------------------------------

    /**
    * @brief A command to delete entities, or re-create them when undone
    *
    * @author Daniel Konings
    */
    class DeleteEntityCommand : public EntityCommand
    {

    public:

      /**
      * @see EntityCommand::EntityCommand
      *
      * @param[in] deleted_index The index this entity was deleted from
      */
      DeleteEntityCommand(
        const foundation::UUID& uuid,
        HierarchyView* view,
        int deleted_index);

      /**
      * @see EntityCommand::RedoImpl
      */
      void RedoImpl() override;

      /**
      * @see EntityCommand::UndoImpl
      */
      void UndoImpl() override;

      /**
      * @brief Makes sure the deleted entity is reparented to its original
      *        entity
      */
      void PostUndo() override;

    private:

      QString serialization_data_; //!< The data of the deleted entity

      foundation::UUID deleted_from_; //!< The entity we were deleted from
      int deleted_index_; //!< The index this entity was deleted from
    };

    //--------------------------------------------------------------------------

    /**
    * @brief Used to re-parent entities or move their position around in
    *        the hierarchy view
    *
    * @author Daniel Konings
    */
    class ReparentEntityCommand : public EntityCommand
    {

    public:

      /**
      * @see EntityCommand::EntityCommand
      *
      * @param[in] from The item this item was moved from
      * @param[in] to The item this item was moved to
      * @param[in] index_from The index from which this item was moved
      * @parampin] index_to The index to which this item was moved
      */
      ReparentEntityCommand(
        const foundation::UUID& uuid,
        HierarchyView* view,
        const foundation::UUID& from,
        const foundation::UUID& to,
        int index_from,
        int index_to);

      /**
      * @see EntityCommand::RedoImpl
      */
      void RedoImpl() override;

      /**
      * @see EntityCommand::UndoImpl
      */
      void UndoImpl() override;

      /**
      * @brief Updates the sort indices from a specific index, shifting them
      *        by a positive value of 1 for everything after the item, or
      *        by a negative value of 1 for everything before
      *
      * @remarks Indices before the index are not shifted in any way
      *
      * @param[in] target The target to shift from
      * @param[in] index The index of the target
      */
      void ShiftSortIndices(const foundation::UUID& target, int index);

    private:

      foundation::UUID from_; //!< The item this item was moved from
      foundation::UUID to_; //!< The item this item was moved to

      int index_from_; //!< The index from which this item was moved
      int index_to_; //!< The index to which this item was moved
    };
  }
}