#pragma once

#include <QTreeView>

class QFileSystemModel;

namespace snuffbox
{
  namespace editor
  {
    /**
    * @brief Used to show the different assets within the current asset build
    *        directory. Useful for navigation to reach imported files
    *
    * @author Daniel Konings
    */
    class AssetTree : public QTreeView
    {
      Q_OBJECT;

    public:

      /**
      * @brief Construct through a widget parent
      *
      * @param[in] build_dir The build directory that this asset tree mirrors
      * @param[in] parent The parent widget of this widget
      */
      AssetTree(const QString& build_dir, QWidget* parent = nullptr);

    protected:

      /**
      * @brief Called when the selection within the tree has changed
      *
      * @param[in] selected The items that were selected
      * @param[in] deselected The items that were deselected
      */
      void selectionChanged(
        const QItemSelection& selected, 
        const QItemSelection& deselected) override;
      
    signals:

      /**
      * @brief Emitted when a new directory is selected within the asset tree
      *
      * @param[in] directory The directory that was selected
      */
      void DirectorySelected(const QString& directory);

    public:

      ~AssetTree();

    private:

      QString build_dir_; //!< The build directory that this asset tree mirrors
      QFileSystemModel* model_; //!< The file system model this tree views
    };
  }
}