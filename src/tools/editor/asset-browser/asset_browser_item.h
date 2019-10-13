#pragma once

#include <tools/compilers/definitions/asset_types.h>

#include <QWidget>

class QLabel;

namespace snuffbox
{
  namespace editor
  {
    class AssetIcon;

    /**
    * @brief An asset browser item that handles all functionality in the asset
    *        browser when an item is clicked
    *
    * The item consists of an AssetIcon and a QLabel showing information
    * visually about a particular asset
    *
    * @author Daniel Konings
    */
    class AssetBrowserItem : public QWidget
    {
      
    public:

      /**
      * @brief Construct an asset browser item through a parent widget, from
      *        a particular asset
      *
      * @remarks The asset name is retrieved from the last part of the path
      *
      * @param[in] type The type of the underlying asset
      * @param[in] path The full path of the underlying asset
      * @param[in] parent The parent widget of this item
      */
      AssetBrowserItem(
        compilers::AssetTypes type, 
        const QString& path, 
        QWidget* parent = nullptr);

    private:

      compilers::AssetTypes type_; //!< The type of the underlying asset
      const QString& path_; //!< The full path of the underlying asset

      AssetIcon* icon_; //!< The asset icon

    public:

      static const QSize kMaxItemSize; // The maximum item size
    };
  }
}