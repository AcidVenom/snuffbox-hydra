#pragma once

#include <QUndoCommand>
#include <QUuid>

namespace snuffbox
{
  namespace editor
  {
    class HierarchyView;

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
  }
}