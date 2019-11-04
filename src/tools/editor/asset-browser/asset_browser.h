#pragma once

#include <tools/compilers/definitions/asset_types.h>

#include <QWidget>

class QSplitter;
class QSettings;
class FlowLayout;
class QFrame;
class QMenu;

namespace snuffbox
{
  namespace editor
  {
    class MainWindow;
    class AssetBrowserItem;
    class AssetTree;

    /**
    * @brief Used to show Snuffbox's assets within a specific directory, from
    *        which you can open import settings or import them into the engine
    *
    * @author Daniel Konings
    */
    class AssetBrowser : public QWidget
    {

      Q_OBJECT;

    public:

      /**
      * @brief Construct through a Qt parent
      *
      * @param[in] main_window The main window this browser belongs to
      * @param[in] source_path The source path to operate on
      * @param[in] build_path The build path to operate on
      * @param[in] parent The parent widget this widget lives in
      */
      AssetBrowser(
        MainWindow* main_window,
        const QString& source_path,
        const QString& build_path, 
        QWidget* parent = nullptr);

      /**
      * @brief Saves the splitter state of the asset browser to restore later
      */
      void SaveState(QSettings* settings);

      /**
      * @brief Loads the splitter state of the asset browser, if available
      */
      void LoadState(const QSettings* settings);

    protected:

      /**
      * @brief Retrieves The current source directory from the current path
      *        within the build directory
      *
      * @param[in] from The build directory to map to the source directory
      *
      * @return The mapped directory
      */
      QString GetCurrentSourceDirectory(const QString& from) const;

      /**
      * @brief Creates a new file or directory name based on the 
      *        files and directories that are within a base directory
      *
      * @param[in] base_dir The base directory
      * @param[in] file_or_dir The new file or directory name
      * @param[in] is_file Is this new name a file or directory?
      *
      * @return The new path, mangled so that it is unique
      */
      static QString GetUniqueFileOrDirectoryName(
        const QString& base_dir, 
        const QString& file_or_dir,
        bool is_file);

    public slots:

      /**
      * @brief Refreshes the asset browser within the current navigation path
      */
      void RefreshBrowser();

      /**
      * @brief Refresh both the asset browser and asset tree
      */
      void Refresh();

      /**
      * @brief Called when a new item is selected
      *
      * @param[in] item The item that was selected
      */
      void OnItemSelect(AssetBrowserItem* item);

      /**
      * @brief Called when a new item is hovered over or when the mouse left
      *        the item's rectangle
      *
      * @param[in] item The item that was hovered
      * @param[in] hovered Is the item still hovered?
      */
      void OnItemHovered(AssetBrowserItem* item, bool hovered);

    protected slots:

      /**
      * @brief Called when a new directory is selected within the asset tree
      *
      * @param[in] directory The new directory
      */
      void OnDirectorySelected(const QString& directory);

      /**
      * @brief Shows a custom context menu for when the user right-clicks
      *        within the clickable browser frame
      *
      * @param[in] pos The position to spawn the context menu at
      */
      void CustomContextMenu(const QPoint& pos);

      /**
      * @brief Used to show a context menu for when the user is hovering over
      *        an asset browser item
      *
      * @param[in] menu The menu to show the context actions in
      */
      void ShowItemContextMenu(QMenu* menu);

      /**
      * @brief Used to show a context menu for when the user is not hovering
      *        over an asset browser item
      *
      * @param[in] menu The menu to show the context actions in
      */
      void ShowRegularContextMenu(QMenu* menu);

      /**
      * @brief Creates a new directory in the source directory, from the
      *        current navigation path
      *
      * @param[in] name The new name of the directory
      */
      void CreateNewSourceDirectory(const QString& name) const;

      /**
      * @brief Creates a new asset in the source directory, from the current
      *        navigation path
      *
      * @param[in] type The type of asset to create
      * @param[in] new_name The name of the asset to create
      */
      void CreateNewAsset(
        compilers::AssetTypes type,
        const QString& new_name) const;

      /**
      * @brief Deletes the currently hovered item
      */
      void DeleteHoveredItem();

      /**
      * @brief Renames the currently hovered item
      */
      void RenameHoveredItem();

    signals:

      /**
      * @brief Emitted when the current asset browser selection has changed
      *
      * @parma[in] new_item The new item
      */
      void SelectionChanged(const AssetBrowserItem* new_item);

    private:

      MainWindow* main_window_; //!< The main window this browser belongs to

      QString source_path_; //!< The current source path
      QString build_path_; //!< The current build path

      /**
      * @brief The current navigation path within the build path
      */
      QString navigation_path_;

      AssetTree* tree_; //!< The tree view to navigate the asset tree
      FlowLayout* asset_list_; //!< The asset list layout
      QSplitter* splitter_; //!< The splitter
      QWidget* browser_frame_; //!< The frame that contains the browser
      AssetBrowserItem* selected_item_; //!< The currently selected item
      AssetBrowserItem* hovered_item_; //!< The currently hovered item
      AssetBrowserItem* last_hovered_item_; //!< The last hovered item

      /**
      * @brief The spacing between asset browser items
      */
      static const int kFlowLayoutSpacing_;

      /**
      * @brief The asset browser splitter key
      */
      static const QString kSettingsAssetBrowserKey_;

      /**
      * @brief The name for a newly created directory
      */
      static const QString kNewDirectoryName_;
    };
  }
}