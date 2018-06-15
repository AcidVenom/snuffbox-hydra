#pragma once

#include <tools/compilers/definitions/asset_types.h>

#include <engine/services/asset_service.h>

#include <foundation/io/path.h>

#include <qobject.h>
#include <qwidget.h>

class QTreeView;
class QGridLayout;
class QFileSystemModel;
class QFrame;
class QLabel;
class QModelIndex;
class QImage;

namespace snuffbox
{
  namespace editor
  {
    class AssetBrowser;

    /**
    * @brief An asset browser item to show in the asset browser
    *
    * The asset browser items have a label and an icon, of which the icon
    * is in correlation to the asset type.
    *
    * The item also contains a reference to the path the item belongs to,
    * so it can be dragged onto areas in the interface where paths are required.
    *
    * @author Daniel Konings
    */
    class AssetBrowserItem : public QWidget
    {

      Q_OBJECT

    public:

      /**
      * @brief Construct through a name
      *
      * @param[in] browser The asset browser this asset belongs to
      * @param[in] name The name of the asset
      * @param[in] relative The relative path to the asset
      * @param[in] full_path The full path to the asset
      * @param[in] type The type of the asset
      */
      AssetBrowserItem(
        AssetBrowser* browser,
        const QString& name,
        const foundation::Path& relative,
        const foundation::Path& full_path,
        compilers::AssetTypes type);

    protected:

      /**
      * @brief Creates the actual underlying widgets
      *
      * @param[in] name The name to put at the label field
      */
      void CreateItem(const QString& name);

      /**
      * @brief Applies the correct selection colors 
      *        to the created label and icon
      *
      * @param[in] selected Is this item currently selected?
      */
      void SetSelected(bool selected);

      /**
      * @brief Overrides the double clicking behavior when clicking on an asset
      */
      void mouseDoubleClickEvent(QMouseEvent* e) override;

      /**
      * @brief Changes the background color of the widget and deselects others
      */
      void mousePressEvent(QMouseEvent* e) override;

    private slots:
      
      /**
      * @brief Called to deselect an asset
      */
      void Deselect();


    private:

      AssetBrowser* browser_; //!< The asset browser this asset belongs to

      foundation::Path relative_; //!< The relative path to the asset
      foundation::Path full_path_; //!< The full path to the asset
      
      compilers::AssetTypes type_; //!< The type of the asset

      QFrame* icon_; //!< The created icon
      QLabel* label_; //!< The created label

      static const int kIconSize_; //!< The squared icon size
      static const int kLabelHeight_; //!< The label heights
    };

    /**
    * @brief The asset browser that shows all assets currently in the build
    *        directory
    *
    * @author Daniel Konings
    */
    class AssetBrowser : public QObject
    {

      Q_OBJECT

    public:

      /**
      * @brief Construct by specifying the widgets the asset browser operates
      *        on
      *
      * @param[in] tree The tree view to show the build directories in
      * @param[in] assets The assets browser layout
      */
      AssetBrowser(QTreeView* tree, QGridLayout* assets);

      /**
      * @brief Refresh the asset browser by path
      *
      * @param[in] path The path to the current build directory
      */
      void Refresh(const foundation::Path& path);

      /**
      * @brief Gets an icon for a specific asset type
      *
      * @param[in] type The type of the asset to retrieve
      *
      * @return The retrieved image, or nullptr if there is no icon for
      *         the type
      */
      QImage* GetIcon(compilers::AssetTypes type);

      /**
      * @brief Used to show the assets in a specified build directory,
      *        called from the QTreeView when the selection changed
      *
      * @param[in] dir The directory to traverse
      */
      void ShowDirectory(const foundation::Path& dir);

    protected:

      /**
      * @brief Adds an asset to the asset browser by path
      *
      * @param[in] path The path to the file to add
      * @param[in] type The asset type of the file
      */
      void AddFromPath(
        const foundation::Path& path, 
        compilers::AssetTypes type);

      /**
      * @brief Adds a new widget, incrementing the column and row count
      *
      * Columns go until AssetBrowser::kItemsPerRow_, after which the current
      * row is also incremented.
      *
      * @param[in] widget The widget to add to the layout
      */
      void AddWidget(QWidget* widget);

      /**
      * @brief Clear the current asset browser layout
      *
      * @remarks This also resets the current row/column count
      */
      void Clear();

      /**
      * @brief Applies the styles for the asset browser
      */
      void ApplyStyle();

      /**
      * @brief Loads all icons
      */
      void LoadIcons();

      /**
      * @brief Creates all QImages out of the icons
      */
      void CreateIcons();

      /**
      * @brief Binds the events for when a new selection is made in the
      *        directory browser
      */
      void BindEvents();

    private slots:

      /**
      * @brief Called when the user changes the directory selection in the
      *        directory tree
      */
      void OnSelectionChanged(const QModelIndex& index);

    signals:

      /**
      * @brief A signal to emit when an asset is double clicked
      *
      * @param[in] relative The relative path to the asset, 
      *                     from the asset folder
      * @param[in] type The type of the asset
      */
      void DoubleClickedAsset(QString relative, int type);

      /**
      * @brief Deselects all assets in the asset browser
      */
      void DeselectAll();

    public:

      /**
      * @brief Frees up the loaded icons
      */
      ~AssetBrowser();

    private:

      /**
      * @brief Sorts assets based on whether they are a directory and
      *        alphabetical order
      *
      * @author Daniel Konings
      */
      struct AssetSorter
      {
        /**
        * @brief The compare function
        */
        bool operator()(
          const engine::AssetService::AssetFile& a, 
          const engine::AssetService::AssetFile& b);
      };

      QString root_; //!< The root directory this asset browser runs in

      QTreeView* tree_; //!< The tree view to show the build directories in
      QGridLayout* assets_; //!< The assets browser layout

      QFileSystemModel* model_; //!< The file system model to show directories

      unsigned int current_row_; //!< The current row we're at
      unsigned int current_column_; //!< The current column we're at

      /**
      * @brief The list of available image icons per asset type
      */
      QImage* icons_[static_cast<int>(compilers::AssetTypes::kCount) + 1];

      static const int kItemsPerRow_; //!< The number of items per row
      static const int kContentMargin_; //!< The margin between assets
    };
  }
}