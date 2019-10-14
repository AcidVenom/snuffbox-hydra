#pragma once

#include <tools/compilers/definitions/asset_types.h>

#include <QWidget>

class QLabel;

namespace snuffbox
{
  namespace editor
  {
    class AssetIcon;
    class AssetBrowser;

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

      Q_OBJECT;
      
    public:

      /**
      * @brief Construct an asset browser item through a parent widget, from
      *        a particular asset
      *
      * @remarks The asset name is retrieved from the last part of the path
      *
      * @param[in] type The type of the underlying asset
      * @param[in] path The full path of the underlying asset
      * @param[in] parent The parent asset browser of this item
      */
      AssetBrowserItem(
        compilers::AssetTypes type, 
        const QString& path, 
        AssetBrowser* parent);

	  protected:

      /**
      * @brief Called when the mouse cursor enters the area of this widget
      *
      * @param[in] evt The event that triggered this event
      */
      void enterEvent(QEvent* evt) override;

      /**
      * @brief Called when the mouse cursor leaves the area of this widget
      *
      * @param[in] evt The event that triggered this event
      */
      void leaveEvent(QEvent* evt) override;

		  /**
		  * @brief Called when the widget is hovered by the mouse and the mouse
      *        button is released
		  *
		  * @param[in] evt The mouse event that triggered this event
		  */
		  void mouseReleaseEvent(QMouseEvent* evt) override;

      /**
      * @brief When sub-classing from QWidget, utilizing Q_OBJECT, stylesheets
      *        are not properly applied anymore, this is here to fix that
      *
      * @param[in] evt The paint event
      */
      void paintEvent(QPaintEvent* evt) override;

      /**
      * @brief Sets whether this asset browser item is selected
      *
      * @param[in] selected The new value
      */
      void SetSelected(bool selected);

      /**
      * @brief Sets whether this asset browser item has been highlighted 
      *        by the mouse cursor
      *
      * @param[in] highlighted The new value
      */
      void SetHighlighted(bool highlighted);

    public slots:

      /**
      * @brief Called when the current selection in the asset browser changed
      *
      * @param[in] new_item The new item
      */
      void OnSelectionChanged(const AssetBrowserItem* new_item);

    signals:

      /**
      * @brief Emitted when this item has been selected
      *
      * @param[in] item The item that was selected (this)
      */
      void Selected(const AssetBrowserItem* item);

    private:

      compilers::AssetTypes type_; //!< The type of the underlying asset
      const QString& path_; //!< The full path of the underlying asset

      AssetIcon* icon_; //!< The asset icon

      bool selected_; //!< Is this asset browser item selected?

    public:

      static const QSize kMaxItemSize; // The maximum item size
    };
  }
}