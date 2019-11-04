#pragma once

#include <QWidget>

namespace snuffbox
{
  namespace engine
  {
    class Entity;
  }

  namespace editor
  {
    class HierarchyView;

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

    private:

      HierarchyView* hierarchy_; //!< The hierarchy view
    };
  }
}