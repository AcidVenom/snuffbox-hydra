#pragma once

#include <foundation/containers/map.h>

#include <QUndoStack>
#include <QTreeWidget>
#include <QModelIndex>

namespace snuffbox
{
  namespace engine
  {
    class Entity;
    class Scene;
  }

  namespace editor
  {
    class EditorApplication;
    class HierarchyViewItem;

    /**
    * @brief The hierarchy view that shows all entities within the currently
    *        opened scene. This allows for deletion, renaming and selection
    *        of entities to modify them. The view is refreshed when any of
    *        the entities within a scene change.
    *
    *        This class also contains the global undo stack. As I am only
    *        looking to make the editing of entities undo-able, to save some
    *        hassle, this is not necessary to be global.
    *
    * @author Daniel Konings
    */
    class HierarchyView : public QTreeWidget
    {

      friend HierarchyViewItem;

      friend class EntityCommand;
      friend class CreateEntityCommand;
      friend class DeleteEntityCommand;
      friend class ReparentEntityCommand;

      Q_OBJECT;

    public:

      /**
      * @brief Used to delay changes within entity commands until they
      *        have finished mutating the data
      *
      * @author Daniel Konings
      */
      struct SceneChangedBlocker
      {
        /**
        * @brief Delete default constructor
        */
        SceneChangedBlocker() = delete;

        /**
        * @brief Construct through the view to block
        *
        * @param[in] view The view to block
        */
        SceneChangedBlocker(HierarchyView* view);

        /**
        * @brief Destructor, re-enable receiving of changes
        */
        ~SceneChangedBlocker();

        HierarchyView* hierarchy; //!< The view this blocker is for
      };

      friend SceneChangedBlocker;

      /**
      * @brief Creates a hierarchy view with the application to listen to
      *
      * @param[in] app The current application
      * @param[in] parent The parent of the hierarchy view, default = nullptr
      */
      HierarchyView(EditorApplication* app, QWidget* parent = nullptr);

    protected:

      /**
      * @brief Called when a scene has changed
      */
      void OnSceneDataChanged(engine::Scene* scene);

      /**
      * @brief Try to add an entity to the hierarchy view, if it already exists
      *        update the entity instead
      *
      * @param[in] ent The entity to check for
      *
      * @return The corresponding hierarchy view item that was created or
      *         updated for this entity
      */
      HierarchyViewItem* TryAddEntity(engine::Entity* ent);

      /**
      * @brief Re-parents an already existing or new hierarchy view item
      *
      * @param[in] item The item to re-parent
      * @param[in] ent The entity that is contained within the item
      * @param[in] was_added Was this item already added before?
      */
      void ReparentItem(
        HierarchyViewItem* item, 
        engine::Entity* ent,
        bool was_added);

      /**
      * @brief Validates the current entities and removes any that are not in
      *        the scene anymore
      *
      * @param[in] current_entities A hash set of all current entities
      */
      void ValidateCurrentEntities(
        foundation::HashSet<engine::Entity*> current_entities);

      /**
      * @return The current scene opened in the scene service
      */
      engine::Scene* GetCurrentScene() const;

      /**
      * @brief Shows the context menu for when no item is being hovered
      *
      * @param[in] pos The position of the context menu
      */
      void ShowRegularContextMenu(const QPoint& pos);

      /**
      * @brief Shows the context menu for when we are hovering an item
      *
      * @param[in] item The item to be modified
      * @param[in] pos The position of the context menu
      */
      void ShowEntityContextMenu(HierarchyViewItem* item, const QPoint& pos);

      /**
      * @brief Searches a hierarchy view item by its index that was assigned
      *        for its entity
      *
      * @param[in] index The index to search for
      *
      * @return The retrieved item, or nullptr if it was not found
      */
      HierarchyViewItem* FindItemByIndex(const QString& index) const;

      /**
      * @brief Searches an entity by its index, as stored in a
      *        hierarchy view item
      *
      * @param[in] index The index to search for
      *
      * @return The retrieved entity, or nullptr if it was not found
      */
      engine::Entity* FindEntityByIndex(const QString& index) const;

      /**
      * @brief Searches for an entity's item by its pointer within the current
      *        entity mapping
      *
      * @param[in] ent The entity to search the item of
      *
      * @return The item, or a nullptr if not found
      */
      HierarchyViewItem* FindItemByEntity(engine::Entity* ent) const;

      /**
      * @brief Refreshes the hierarchy view for the current scene
      */
      void RefreshForCurrentScene();

      /**
      * @brief Updates all retrieval index strings
      */
      void UpdateIndices();

      /**
      * @brief Clears the hierarchy view and all mappings
      */
      void Clear();

      /**
      * @brief Called when an item was dropped onto another item or the root
      *
      * @param[in] evt The drop event
      */
      void dropEvent(QDropEvent* evt) override;

      /**
      * @brief Called after the command from "Create entity" resolves
      *
      * @param[in] index The index the entity should reside at, or -1 to append
      *                  it at the end of the list
      *
      * @return The created entity item
      */
      HierarchyViewItem* CreateNewEntity(int index = -1);

    protected slots:

      /**
      * @brief The custom context menu to add, remove or rename entities
      *
      * @param[in] pos The position to execute the context menu at
      */
      void CustomContextMenu(const QPoint& pos);

      /**
      * @brief Called when an item in the tree was changed (probably a rename)
      *
      * @param[in] item The item that was changed
      * @param[in] column The column of the item that changed
      */
      void OnItemChanged(QTreeWidgetItem* item, int column);

      /**
      * @brief Called when an 'Create entity' has been clicked in the context
      *        menu
      */
      void OnCreateEntity();

      /**
      * @brief Called when 'Delete' has been clicked in the context
      *        menu of an entity
      */
      void OnDeleteEntity();

    public slots:

      /**
      * @brief Called when the asset importer has switched scenes
      *
      * @param[in] scene_name The new name of the scene, could be empty if
      *                       it is a new scene
      */
      void OnSceneChanged(const QString& scene_name);
      
      /**
      * @brief Redo the last entity command
      */
      void Redo();

      /**
      * @brief Undo the last entity command
      */
      void Undo();

    private:

      using EntityMap = foundation::UMap<engine::Entity*, HierarchyViewItem*>;

      EditorApplication* app_; //!< The current application
      EntityMap entity_to_item_; //!< The entity to item mapping

      bool block_scene_changed_; //!< Blocks the scene changed callback

      QUndoStack undo_stack_; //!< The undo stack

      HierarchyViewItem* hovered_item_; //!< The currently hovered item
    };
  }
}