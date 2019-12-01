#pragma once

#include <tools/compilers/definitions/asset_types.h>

#include <QWidget>

class QLabel;

namespace snuffbox
{
  namespace editor
  {
    /**
    * @brief Used to show an icon and name for different assets within the
    *        asset browser
    *
    * @author Daniel Konings
    */
    class AssetIcon : public QWidget
    {

      Q_OBJECT;

    public:

      /**
      * @brief Construct through a Qt parent
      *
      * @param[in] type The type of the asset
      * @param[in] path The full path to the asset
      * @param[in] parent The parent widget this widget lives in
      *
      * @remarks The full path will be split to retrieve the asset name
      */
      AssetIcon(
        compilers::AssetTypes type, 
        const QString& path, 
        QWidget* parent = nullptr);

      /**
      * @brief Changes the type of this asset icon and changes the icon itself
      *        accordingly
      *
      * @param[in] type The type to set
      */
      void SetType(compilers::AssetTypes type);

      /**
      * @brief Splits the path and retrieves its last part without an extension
      *        to use as an asset name and sets the name label
      *
      * @param[in] path The full path to the asset
      */
      void SetNameFromPath(const QString& path);

      /**
      * @return The asset type of this asset icon
      */
      compilers::AssetTypes type() const;

      /**
      * @return The path of this asset icon
      */
      const QString& path() const;

      /**
      * @brief Used to set the icon size of the asset, the icon is always
      *        scaled uniformly, thus only one value is required
      *
      * @param[in] size The uniform size to set
      */
      void SetIconSize(int size);

      /**
      * @brief Visually sets if this asset icon is currently selected
      *
      * @param[in] selected Is this icon selected?
      */
      void SetSelected(bool selected);

      /**
      * @return The icon size of this asset icon
      */
      int icon_size() const;

    protected:

      /**
      * @brief Loads all available icons into memory, using Snuffbox's
      *        virtual file system
      */
      static void LoadAllIcons();

      /**
      * @brief Converts an asset type to its respective asset icon
      *
      * @param[in] type The type to convert
      *
      * @return The image, or nullptr retrieving of the image from the virtual
      *         file system failed
      *
      * @remarks The asset icons are statically created, so that they don't
      *          have to be re-created every time an icon is created
      */
      static const QImage* AssetTypesToIcon(compilers::AssetTypes type);

    private:

      compilers::AssetTypes type_; //!< The type of this asset icon
      QString path_; //!< The path of this asset icon
      int icon_size_; //!< The icon size of this asset icon

      QLabel* icon_label_; //!< The label to display the icon with
      QLabel* name_label_; //!< The label to display the asset name with

      static const int kDefaultIconSize_; //!< The default icon size

      /**
      * @brief The amount of elision padding for the label text
      *        e.g. 'some very long' name becomes 'some ver..'
      */
      static const int kElisionPadding_;

      /**
      * @brief The padding for the name labels
      */
      static const int kLabelPadding_;
    };
  }
}