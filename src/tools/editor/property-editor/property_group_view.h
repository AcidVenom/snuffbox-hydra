#pragma once

#include "tools/editor/property-editor/property_mappings.h"

#include <QWidget>
#include <QString>

namespace snuffbox
{
  namespace editor
  {
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
      * @param[in] object The object to operate on
      * @param[in] parent The parent of this widget, default = nullptr
      */
      PropertyGroupView(
        const QString& header, 
        const PropertyMap& map,
        void* object,
        QWidget* parent = nullptr);

    private:

      static const int kHeaderHeight_; //!< The height of the header
      static const int kHeaderPadding_; //!< The padding of the header
      static const int kInnerPadding_; //!< The inner padding of each edit
      static const int kInnerSpacing_; //!< The spacing between each edit
    };
  }
}