#pragma once

#include <foundation/containers/vector.h>

#include <QWidget>

class QVBoxLayout;
class QPushButton;

namespace snuffbox
{
  namespace engine
  {
    class Entity;
  }

  namespace editor
  {
    class HierarchyView;
    class PropertyGroupView;

    /**
    * @brief Used to show a multitude of PropertyValueEdits based on the
    *        property mappings of both entities and their respective components
    *
    * The property view should contain elements whenever an entity is selected
    * and only update within that timeframe. After an entity is deselected
    * it should remove all of its listed entries.
    *
    * @author Daniel Konings
    */
    class PropertyView : public QWidget
    {

      Q_OBJECT;

    public:

      /**
      * @brief Constructs the property view with a parent
      *
      * @param[in] hierarchy The hierarchy to listen to for entity changes
      * @param[in] parent The parent of this view, default = nullptr
      */
      PropertyView(HierarchyView* hierarchy, QWidget* parent = nullptr);

      /**
      * @brief Shows a property view for an entity
      *
      * This will also show all property edits for the components contained
      * within the entity. PropertyView::Clear should be called after the
      * entity was deselected.
      *
      * This pushes commands onto the undo stack to edit the properties of
      * an entity.
      *
      * @param[in] ent The entity to show the properties of
      */
      void ShowForEntity(engine::Entity* ent);

      /**
      * @brief Updates all current property value edits if they have been
      *        changed
      */
      void Update();

    protected:

      /**
      * @brief Clears the property view after deselection
      */
      void Clear();

    protected slots:

      /**
      * @brief Called when a new component has been added through the search
      *        dialog
      *
      * @param[in] index The index of the component in the search dialog, which
      *                  is a direct mapping to the component + 1, as we skip
      *                  Transform components
      */
      void OnAddComponent(int index);

    private:

      HierarchyView* hierarchy_; //!< The hierarchy view

      /**
      * @brief The list of property group views to update accordingly
      */
      foundation::Vector<PropertyGroupView*> views_;

      QWidget* frame_; //!< The main frame of this widget
      QVBoxLayout* layout_; //!< The main layout of this widget
      QPushButton* add_component_; //!< The "add component" button

      engine::Entity* entity_; //!< The current entity
      int old_component_count_; //!< The old component count of the entity
    };
  }
}