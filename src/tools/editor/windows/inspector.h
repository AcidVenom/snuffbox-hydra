#pragma once

#include <engine/ecs/entity.h>

#include <qobject.h>
#include <qtreewidget.h>

namespace snuffbox
{
  namespace engine
  {
    class Entity;
  }

  namespace editor
  {
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
      *
      * @return The created widget to be shown
      */
      template <engine::Components C>
      QWidget* ShowComponent(engine::IComponent* component);

    signals:

      /**
      * @brief Used to refresh the hierarchy
      */
      void RefreshHierarchy();

    private:

      QTreeWidget* tree_; //!< The tree to append inspector fields to
    };

    //--------------------------------------------------------------------------
    template <engine::Components C>
    inline void Inspector::ShowComponents(
      engine::Entity* entity, 
      QTreeWidgetItem* top)
    {
      QTreeWidgetItem* new_item = nullptr;

      const foundation::Vector<engine::IComponent*>& components = 
        entity->GetComponents(C);

      QWidget* widget = nullptr;
      QColor col = EditorColors::DockColor();

      for (size_t j = 0; j < components.size(); ++j)
      {
        new_item = new QTreeWidgetItem();
        new_item->setBackgroundColor(0, col);
        widget = ShowComponent<C>(components.at(j));

        if (widget != nullptr)
        {
          new_item->setSizeHint(0, widget->sizeHint());

          if (top == nullptr)
          {
            tree_->addTopLevelItem(new_item);
          }
          else
          {
            top->addChild(new_item);
          }

          tree_->setItemWidget(new_item, 0, widget);
        }
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