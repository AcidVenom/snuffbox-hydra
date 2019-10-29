#pragma once

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
      * @brief Construct by index and the hierarchy view this command got
      *        created from
      *
      * @param[in] index The index of the entity to operate on
      * @param[in] view The hierarchy view that invoked this command
      */
      EntityCommand(const QString& index, HierarchyView* view);

      /**
      * @return The hierarchy view that invoked this command
      */
      HierarchyView* view() const;

      /**
      * @return The index of the entity to operate on
      */
      const QString& index() const;

      /**
      * @brief Called when this command is executed initially or redone
      *
      * @remarks This makes sure any refreshes on the hierarchy view
      *          are suspressed and that indexing strings are updated
      */
      void redo() override;

      /**
      * @brief Called when this entity command is undone
      *
      * @remarks This makes sure any refreshes on the hierarchy view
      *          are suspressed and that indexing strings are updated
      */
      void undo() override;

      /**
      * @brief Called when this command is executed initially or redone
      */
      virtual void RedoImpl() = 0;

      /**
      * @brief Called when this entity command is undone
      */
      virtual void UndoImpl() = 0;

      /**
      * @brief Retrieves the hierarchy view item from its index
      *
      * @param[in] index The index to retrieve
      *
      * @return The found item, or nullptr if it doesn't exist, or the index
      *         was not valid
      */
      HierarchyViewItem* GetFromIndex(const QString& index) const;

      /**
      * @return The entity this entity command operates on
      */
      HierarchyViewItem* GetSelf() const;

    private:

      QString index_; //!< The index of the entity to operate on
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
      CreateEntityCommand(const QString& index, HierarchyView* view);

      /**
      * @see EntityCommand::redo
      */
      void RedoImpl() override;

      /**
      * @see EntityCommand::undo
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
      * @param[in] deleted_from The index this entity was deleted from
      */
      DeleteEntityCommand(
        const QString& index,
        HierarchyView* view,
        int deleted_from);

      /**
      * @see EntityCommand::redo
      */
      void RedoImpl() override;

      /**
      * @see EntityCommand::undo
      */
      void UndoImpl() override;

    private:

      QString serialization_data_; //!< The data of the deleted entity
      int deleted_from_; //!< The index this entity was deleted from
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
        const QString& index,
        HierarchyView* view,
        const QString& from,
        const QString& to,
        int index_from,
        int index_to);

      /**
      * @see EntityCommand::redo
      */
      void RedoImpl() override;

      /**
      * @see EntityCommand::undo
      */
      void UndoImpl() override;

    private:

      QString from_model_idx_; //!< The item this item was moved from
      QString to_model_idx_; //!< The item this item was moved to

      int index_from_; //!< The index from which this item was moved
      int index_to_; //!< The index to which this item was moved
    };
  }
}