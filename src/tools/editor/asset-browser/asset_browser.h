#pragma once

#include <QWidget>

class QTreeView;
class QGridLayout;

namespace snuffbox
{
  namespace editor
  {
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
      * @param[in] parent The parent widget this widget lives in
      */
      AssetBrowser(QWidget* parent = nullptr);

    public slots:

      /**
      * @brief Refreshes the asset browser to a new directory
      *
      * @param[in] path The path of the new directory to open
      */
      void Refresh(const QString& path);

    private:

      QTreeView* tree_; //!< The tree view to navigate the asset tree
      QGridLayout* asset_list_; //!< The asset list layout

      static const int kItemsPerRow_; //!< The number of items per row
    };
  }
}