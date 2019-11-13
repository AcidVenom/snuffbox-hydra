#pragma once

#include "tools/editor/property-editor/property_mappings.h"

#include <foundation/containers/vector.h>

#include <QWidget>
#include <QString>

namespace snuffbox
{
  namespace engine
  {
    class Entity;
    class IComponent;
  }

  namespace editor
  {
    class HierarchyView;
    class PropertyValueEdit;

    /**
    * @brief Used to group multiple PropertyValueEdit objects together
    *        for e.g. an entity or component, to be used in the PropertyView
    *
    * @author Daniel Konings
    */
    class PropertyGroupView : public QWidget
    {

      Q_OBJECT;

    public:

      /**
      * @brief Construct with a specific header name to show at the top
      *        of the group and the property map to list values of
      *
      * @param[in] header The header text
      * @param[in] map The map to retrieve property values from
      * @param[in] entity The entity to operate on
      * @param[in] component The component to operate on
      * @param[in] parent The parent of this widget, default = nullptr
      */
      PropertyGroupView(
        HierarchyView* hierarchy,
        const QString& header, 
        const PropertyMap& map,
        engine::Entity* entity,
        engine::IComponent* component,
        QWidget* parent = nullptr);

      /**
      * @brief Updates all values currently in the view
      */
      void UpdateValues();

    private:

      foundation::Vector<PropertyValueEdit*> edits_; //!< All value edits

      static const int kHeaderHeight_; //!< The height of the header
      static const int kHeaderPadding_; //!< The padding of the header
      static const int kInnerPadding_; //!< The inner padding of each edit
      static const int kInnerSpacing_; //!< The spacing between each edit
    };
  }
}