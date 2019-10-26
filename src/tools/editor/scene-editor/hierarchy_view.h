#pragma once

#include <foundation/containers/map.h>

#include <QUndoStack>
#include <QTreeWidget>
#include <QUuid>

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

      friend class CreateEntityCommand;

      Q_OBJECT;

    public:

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
      * @param[in] uuid The UUID to assign to this entity
      * @param[in] update_uuid Should we update the UUID if already found?
      */
      void TryAddEntity(
        engine::Entity* ent, 
        const QUuid& uuid, 
        bool update_uuid);

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
      * @brief Searches a hierarchy view item by its UUID that was assigned
      *        for its entity
      *
      * @param[in] uuid The UUID to search for
      *
      * @return The retrieved item, or nullptr if it was not found
      */
      HierarchyViewItem* FindItemByUUID(const QUuid& uuid) const;

      /**
      * @brief Searches an entity by its UUID, as stored in a
      *        hierarchy view item
      *
      * @param[in] uuid The UUID to search for
      *
      * @return The retrieved entity, or nullptr if it was not found
      */
      engine::Entity* FindEntityByUUID(const QUuid& uuid) const;

      /**
      * @brief Refreshes the hierarchy view for the current scene
      */
      void RefreshForCurrentScene();

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
      * @param[in] uuid The UUID of the command, which will be the new UUID
      *                 of the created entity
      */
      void CreateNewEntity(const QUuid& uuid);

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

      QUndoStack undo_stack_; //!< The undo stack
    };
  }
}