#pragma once

#include <foundation/io/path.h>

#include <qobject.h>

class QTreeView;
class QLayout;
class QFileSystemModel;

namespace snuffbox
{
  namespace editor
  {
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
      AssetBrowser(QTreeView* tree, QLayout* assets);

      /**
      * @brief Refresh the asset browser by path
      *
      * @param[in] path The path to the current build directory
      */
      void Refresh(const foundation::Path& path);

    private:

      QTreeView* tree_; //!< The tree view to show the build directories in
      QLayout* assets_; //!< The assets browser layout

      QFileSystemModel* model_; //!< The file system model to show directories
    };
  }
}