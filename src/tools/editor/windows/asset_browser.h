#pragma once

#include <foundation/io/path.h>

#include <qobject.h>
#include <qwidget.h>

class QTreeView;
class QGridLayout;
class QFileSystemModel;
class QFrame;
class QLabel;

namespace snuffbox
{
  namespace editor
  {
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

    public:

      /**
      * @brief Construct through a name
      *
      * @param[in] name The name of the asset
      */
      AssetBrowserItem(const QString& name);

    protected:

      /**
      * @brief Creates the actual underlying widgets
      *
      * @param[in] name The name to put at the label field
      */
      void CreateItem(const QString& name);

      /**
      * @brief Applies the colors to the created label and icon
      */
      void ApplyStyle();

    private:

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

    protected:

      /**
      * @brief Debug functionality to test the asset browser interface
      */
      void TestAssets();

      /**
      * @brief Applies the styles for the asset browser
      */
      void ApplyStyle();

    private:

      QTreeView* tree_; //!< The tree view to show the build directories in
      QGridLayout* assets_; //!< The assets browser layout

      QFileSystemModel* model_; //!< The file system model to show directories

      static const int kItemsPerRow_; //!< The number of items per row
      static const int kContentMargin_; //!< The margin between assets
    };
  }
}