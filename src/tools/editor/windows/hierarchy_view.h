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
      * @return The current scene retrieved from the scene service
      */
      engine::Scene* CurrentScene();

      /**
      * @brief Adds an entity to the current scene
      */
      void AddEntity();

      /**
      * @brief Removes an entity from the current scene
      */
      void RemoveEntity(HierarchyViewItem* item);

      /**
      * @brief Updates the scene inspector with the current transform
      *        hierarchy
      *
      * @see Scene::UpdateHierarchy
      */
      void OnHierarchyChanged();

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

    private slots:

      /**
      * @brief Called when a new context menu has to be shown
      */
      void OnCustomContextMenu(const QPoint& p);

      /**
      * @brief Called when an item changed, which implies a rename
      */
      void OnRenameItem(QTreeWidgetItem* item, int column);

    private:

      QTreeWidget* tree_; //!< The underlying Qt tree widget
      QMenu* context_menu_; //!< The context menu
      QAction* add_entity_; //!< The add entity option
      QAction* remove_entity_; //!< The remove entity option
      MainWindow* window_; //!< The window that owns this hierarchy
      engine::SceneService* scene_service_; //!< The current scene service
    };
  }
}