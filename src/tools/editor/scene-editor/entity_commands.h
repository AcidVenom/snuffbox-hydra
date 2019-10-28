#pragma once

#include <QUndoCommand>
#include <QUuid>

namespace snuffbox
{
  namespace editor
  {
    class HierarchyView;
    class HierarchyViewItem;

    /**
    * @brief The base class of every command that does something with entities.
    *        This class guarantees that we have a reference to the hierarchy
    *        view and that we have a valid UUID to operate with.
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
      EntityCommand(const QUuid& uuid, HierarchyView* view);

      /**
      * @return The hierarchy view that invoked this command
      */
      HierarchyView* view() const;

      /**
      * @return The UUID of the entity to operate on
      */
      const QUuid& uuid() const;

      /**
      * @brief Called when this command is executed initially or redone
      */
      virtual void redo() override = 0;

      /**
      * @brief Called when this entity command is undone
      */
      virtual void undo() override = 0;

    private:

      QUuid uuid_; //!< The UUID of the entity to operate on
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
      CreateEntityCommand(const QUuid& uuid, HierarchyView* view);

      /**
      * @see EntityCommand::redo
      */
      void redo() override;

      /**
      * @see EntityCommand::undo
      */
      void undo() override;
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
      * @param[in] deleted_from The index this entity was deleted from
      */
      DeleteEntityCommand(
        const QUuid& uuid,
        HierarchyView* view,
        int deleted_from);

      /**
      * @see EntityCommand::redo
      */
      void redo() override;

      /**
      * @see EntityCommand::undo
      */
      void undo() override;

    private:

      QString serialization_data_; //!< The data of the deleted entity
      int deleted_from_; //!< The index this entity was deleted from
    };

    //--------------------------------------------------------------------------
    class ReparentEntityCommand : public EntityCommand
    {
      ReparentEntityCommand(
        const QUuid& uuid,
        HierarchyView* view,
        HierarchyViewItem* from,
        HierarchyViewItem* to);

      void redo() override;

      void undo() override;

    protected:

      int GetIndexForItem(HierarchyViewItem* item) const;

    private:

      HierarchyViewItem* from_;
      HierarchtViewItem* to_;

      int index_from_;
      int index_to_;
    };
  }
}