#pragma once

#include <QWidget>

#include <engine/assets/asset.h>

class QPushButton;

namespace snuffbox
{
  namespace editor
  {
    /**
    * @brief Used to edit asset properties by searching through the list
    *        of available assets for that particular type
    *
    * @see SearcherWidget
    *
    * @author Daniel Konings
    */
    class PropertyAssetEdit : public QWidget
    {

      Q_OBJECT;

    public:

      /**
      * @brief Creates a property asset edit with a parent
      *
      * @param[in] parent The parent of this widget, default = nullptr
      */
      PropertyAssetEdit(QWidget* parent = nullptr);

      /**
      * @brief Sets the value of this asset edit, this will also be used
      *        to determine the type of serializable asset to use when searching
      *
      * @param[in] asset The asset to set
      */
      void set_asset(const engine::SerializableAsset& asset);

      /**
      * @return The current serializable asset
      */
      const engine::SerializableAsset& asset() const;

    protected:
      
      /**
      * @brief Checks if the currently set asset is valid, setting a red color
      *        if it's not, a green color if it is, or else the default gray
      */
      void UpdateVisuals();

    protected slots:

      /**
      * @brief Show the search dialog for when the user clicked the input box
      */
      void ShowSearcher();

      /**
      * @brief Called when the asset in the edit has been cleared
      */
      void OnClear();

    signals:

      /**
      * @brief Emitted when a new asset has been selected in the asset searcher
      *
      * @param[in] asset_path The path of the new asset
      */
      void AssetChanged(const QString& asset_path);

      /**
      * @brief Called when a reference to and asset has been cleared
      */
      void AssetCleared();

    private:

      engine::SerializableAsset asset_; //!< The current serializable asset

      QPushButton* input_; //!< The input button

      static const int kSearchBoxWidth_; //!< The width of the search box
      static const int kClearButtonWidth_; //!< The width of the clear button
    };
  }
}