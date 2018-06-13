#pragma once

#include "tools/editor/definitions/editor_colors.h"

#include <engine/ecs/entity.h>

#include <qobject.h>
#include <qtreewidget.h>

class QMenu;
class QAction;

namespace snuffbox
{
  namespace engine
  {
    class Entity;
  }

  namespace editor
  {
    /**
    * @brief A tree widget item that contains a reference to the corresponding
    *        component
    *
    * This is used to poll the component data after it has been updated from
    * within the engine (either in scripting or C++), so that the values can
    * be synchronized without rebuilding the entire inspector window.
    *
    * Furthermore this is used to remove components that are on the entity
    *
    * @author Daniel Konings
    */
    class InspectorComponentItem : public QTreeWidgetItem
    {
      
    public:

      /**
      * @brief Construct through an underlying type and component
      *
      * @param[in] type The component type to store
      * @param[in] component The component to store
      */
      InspectorComponentItem(
        engine::Components type, 
        engine::IComponent* component);

      /**
      * @return The component type stored in this item
      */
      engine::Components type() const;

      /**
      * @return The component stored in this item
      */
      engine::IComponent* component() const;

    private:

      engine::Components type_; //!< The component type stored in this item
      engine::IComponent* component_; //!< The component stored in this item
    };

    /**
    * @brief The inspector window to inspect entities and their components with
    *
    * The inspector will also show the custom editor fields that can be
    * edited to adjust an entity's properties.
    *
    * @author Daniel Konings
    */
    class Inspector : public QObject
    {

      Q_OBJECT

    public:

      /**
      * @brief Construct the inspector wrapper through its main widget
      *
      * @param[in] widget The main tree widget this inspector will operate on
      */
      Inspector(QTreeWidget* widget);

      /**
      * @brief Shows the data contained within an entity, including
      *        its components
      *
      * @param[in] entity The entity to show in the inspector
      */
      void ShowEntity(engine::Entity* entity);

      /**
      * @return The selected entity
      */
      engine::Entity* selected() const;

      /**
      * @brief Shows all typed components of an entity in the inspector
      *
      * @remarks This method should be called with the first occurence
      *          in the Components enumerator, as it is unrolled at compile-time
      *
      * The loop unrolls until Components::kCount is found
      *
      * @param[in] entity The entity to show the components of
      * @param[in] top The top level widget to child the new entries to;
      *                if nullptr they are added as a top level item
      */
      template <engine::Components C = engine::Components::kTransform>
      void ShowComponents(
        engine::Entity* entity, 
        QTreeWidgetItem* top = nullptr);

      /**
      * @brief Shows a typed component of an entity in the inspector
      *
      * @tparam C The component type, specialize this to show the components
      *
      * @param[in] component The component to show
      * @param[in] parent The parent widget for this component
      *
      * @return The created widget to be shown
      */
      template <engine::Components C>
      QWidget* ShowComponent(
        engine::IComponent* component, 
        QTreeWidgetItem* parent);

    protected:

      /**
      * @brief Applies the style of the inspector
      */
      void ApplyStyle();

      /**
      * @brief Adds the context menu to remove components
      */
      void AddContextMenu();

    private slots:

      /**
      * @brief Called when the context menu is requested 
      */
      void OnCustomContextMenu(const QPoint& p);

    public:

      /**
      * @brief Clean up the created context menu
      */
      ~Inspector();

    private:

      QTreeWidget* tree_; //!< The tree to append inspector fields to

      QMenu* context_menu_; //!< The context menu of the inspector
      QAction* remove_component_; //!< The remove component action

      engine::Entity* selected_; //!< The selected entity
    };

    //--------------------------------------------------------------------------
    template <engine::Components C>
    inline void Inspector::ShowComponents(
      engine::Entity* entity, 
      QTreeWidgetItem* top)
    {
      InspectorComponentItem* new_item = nullptr;
      InspectorComponentItem* sub_item = nullptr;
      engine::IComponent* component = nullptr;
      QWidget* sub_widget = nullptr;
      QSize size_hint{ -1, 24 };

      QString name;

      const foundation::Vector<engine::IComponent*>& components = 
        entity->GetComponents(C);

      const QPalette& palette = EditorColors::DefaultPalette();
      const QColor& bg_col_top = EditorColors::DockColor();
      const QColor& bg_col_sub = palette.color(QPalette::Window);

      for (size_t j = 0; j < components.size(); ++j)
      {
        component = components.at(j);
        new_item = new InspectorComponentItem(C, component);

        sub_widget = ShowComponent<C>(component, new_item);
        new_item->setBackgroundColor(0, bg_col_top);
        new_item->setSizeHint(0, size_hint);

        if (sub_widget != nullptr)
        {
          sub_item = new InspectorComponentItem(C, component);
          sub_item->setSizeHint(0, sub_widget->sizeHint());
          sub_item->setBackgroundColor(0, bg_col_sub);

          new_item->addChild(sub_item);
          tree_->setItemWidget(sub_item, 0, sub_widget);
        }

        if (top == nullptr)
        {
          tree_->addTopLevelItem(new_item);
        }
        else
        {
          top->addChild(new_item);
        }

        new_item->setExpanded(true);
      }

      const int next = static_cast<int>(C) + 1;
      ShowComponents<static_cast<engine::Components>(next)>(entity, top);
    }

    //--------------------------------------------------------------------------
    template <>
    inline void Inspector::ShowComponents<engine::Components::kCount>(
      engine::Entity* entity,
      QTreeWidgetItem* top)
    {

    }
  }
}
