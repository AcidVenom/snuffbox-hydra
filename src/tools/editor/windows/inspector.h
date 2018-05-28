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

    signals:

      /**
      * @brief Used to refresh the hierarchy
      */
      void RefreshHierarchy();

    private:

      bool refreshing_; //!< Are we refreshing the inspector?
      QTreeWidget* tree_; //!< The tree to append inspector fields to
    };

    //--------------------------------------------------------------------------
    template <engine::Components C>
    inline void Inspector::ShowComponents(
      engine::Entity* entity, 
      QTreeWidgetItem* top)
    {
      QTreeWidgetItem* new_item = nullptr;
      QTreeWidgetItem* sub_item = nullptr;
      QWidget* widget = nullptr;
      QSize size_hint{ -1, 24 };

      QString name;

      const foundation::Vector<engine::IComponent*>& components = 
        entity->GetComponents(C);

      const QPalette& palette = EditorColors::DefaultPalette();
      const QColor& bg_col_top = EditorColors::DockColor();
      const QColor& bg_col_sub = palette.color(QPalette::Window);

      for (size_t j = 0; j < components.size(); ++j)
      {
        new_item = new QTreeWidgetItem();

        widget = ShowComponent<C>(components.at(j), new_item);
        new_item->setBackgroundColor(0, bg_col_top);
        new_item->setSizeHint(0, size_hint);

        if (widget != nullptr)
        {
          sub_item = new QTreeWidgetItem();
          sub_item->setSizeHint(0, widget->sizeHint());
          sub_item->setBackgroundColor(0, bg_col_sub);

          new_item->addChild(sub_item);
          tree_->setItemWidget(sub_item, 0, widget);
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