#pragma once

#include <qobject.h>
#include <qtreewidget.h>

class QTreeWidget;
class QTreeWidgetItem;
class QMenu;
class QAction;

namespace snuffbox
{
  namespace engine
  {
    class TransformComponent;
    class Scene;
    class SceneService;
    class Entity;
  }

  namespace editor
  {
    class MainWindow;

    /**
    * @brief Used to contain transform pointers within the QTreeWidgetItems
    *        stored in the HierarchyView
    *
    * @author Daniel Konings
    */
    class HierarchyViewItem : public QTreeWidgetItem
    {

    public:

      /**
      * @brief Stores a transform within this tree item
      *
      * @remarks The name of the entity is automatically assigned to the tree
      *          item, received from the TransformComponent
      *
      * @param[in] transform The transform to store
      */
      HierarchyViewItem(engine::TransformComponent* transform);

      /**
      * @return The stored transform
      */
      engine::TransformComponent* transform() const;

    private:

      engine::TransformComponent* transform_; //!< The stored transform
    };

    /**
    * @brief The hierarchy view that handles creation, deletion and parenting
    *        of entities and their transforms
    *
    * The hierarchy view is updated through the engine-sided scene
    *
    * @author Daniel Konings
    */
    class HierarchyView : public QObject
    {

      Q_OBJECT

    public:

      /**
      * @brief Construct a hierarchy view wrapper around a tree view
      *
      * @param[in] widget The tree view widget to fill data in
      * @param[in] scene_service The scene service to sync this view with
      */
      HierarchyView(QTreeWidget* widget, engine::SceneService* scene_service);

    protected:

      /**
      * @brief Adds the context menu to the hierarchy view
      */
      void AddContextMenu();

      /**
      * @brief Bind the events for communication between the different widgets
      */
      void BindEvents();

      /**
      * @return The current scene retrieved from the scene service
      */
      engine::Scene* CurrentScene();

      /**
      * @brief Adds an entity to the current scene
      *
      * @param[in] item The item to parent the new entity to, or nullptr if
      *                 created as a root
      */
      void AddEntity(HierarchyViewItem* item);

      /**
      * @brief Removes an entity from the current scene from a tree item
      *
      * @param[in] item The tree item to retrieve the entity from
      */
      void RemoveEntity(HierarchyViewItem* item);

      /**
      * @brief Adds a transform component as a child item of the hierarchy
      *
      * @param[in] child The child to add
      * @param[in] item The parent item, if nullptr the item is added as a top
      *                 level item
      */
      void AddSceneChild(
        engine::TransformComponent* child,
        HierarchyViewItem* item = nullptr);

      /**
      * @brief Handles a drag start event received from the viewport's event
      *        filter
      *
      * This method will set the current dragging item to the widget that we
      * started dragging on
      *
      * @param[in] evt The drag enter event
      */
      void OnDragEnter(QDragEnterEvent* evt);

      /**
      * @brief Handles a drop event received from the viewport's event filter
      *
      * This method will reparent the dropped item to the item it was dropped
      * on, if any. Otherwise, it will rearrange the hierarchy.
      *
      * @param[in] evt The drop event
      */
      void OnDropEvent(QDropEvent* evt);

      /**
      * @brief Called when a new item was selected in the hierarchy
      */
      void OnSelectionChanged();

      /**
      * @brief The event filter to enable custom drag and drop behavior
      */
      bool eventFilter(QObject* obj, QEvent* evt) override;

    private slots:

      /**
      * @brief Called when a new context menu has to be shown
      */
      void OnCustomContextMenu(const QPoint& p);

      /**
      * @brief Called when an item changed, which implies a rename
      */
      void OnRenameItem(QTreeWidgetItem* item, int column);

      /**
      * @brief Updates the scene inspector with the current transform
      *        hierarchy
      *
      * @see Scene::UpdateHierarchy
      */
      void OnHierarchyChanged();

    signals:

      /**
      * @brief Used to send a signal when the entity selection was changed
      */
      void SelectEntity(engine::Entity*);

    public:

      /**
      * @brief Clean up the context menu
      */
      ~HierarchyView();

    private:

      QTreeWidget* tree_; //!< The underlying Qt tree widget

      QMenu* context_menu_; //!< The context menu
      QAction* add_entity_; //!< The add entity option
      QAction* remove_entity_; //!< The remove entity option

      MainWindow* window_; //!< The window that owns this hierarchy
      engine::SceneService* scene_service_; //!< The current scene service

      HierarchyViewItem* dragged_; //!< The currently dragged item
    };
  }
}