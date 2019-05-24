#include "tools/editor/asset-browser/asset_browser.h"
#include "tools/editor/asset-browser/asset_browser_item.h"

#include <engine/services/asset_service.h>

#include <QTreeView>
#include <QGridLayout>
#include <QSplitter>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    const int AssetBrowser::kItemsPerRow_ = 5;

    //--------------------------------------------------------------------------
    AssetBrowser::AssetBrowser(QWidget* parent) :
      QWidget(parent),
      tree_(nullptr),
      asset_list_(nullptr)
    {
      setObjectName(QStringLiteral("AssetBrowser"));

      QHBoxLayout* main_layout = new QHBoxLayout();

      tree_ = new QTreeView(this);
      tree_->setObjectName(QStringLiteral("AssetBrowserTree"));

      QFrame* browser_frame = new QFrame(this);
      browser_frame->setObjectName(QStringLiteral("AssetBrowserFrame"));
      browser_frame->setFrameStyle(
        QFrame::Shape::StyledPanel | QFrame::Shadow::Sunken);

      asset_list_ = new QGridLayout(browser_frame);
      asset_list_->setObjectName(QStringLiteral("AssetBrowserListLayout"));

      browser_frame->setLayout(asset_list_);

      QSplitter* splitter = new QSplitter(this);
      splitter->setObjectName(QStringLiteral("AssetBrowserSplitter"));
      splitter->setOrientation(Qt::Orientation::Horizontal);

      splitter->addWidget(tree_);
      splitter->addWidget(browser_frame);

      main_layout->addWidget(splitter);
      
      setLayout(main_layout);
    }

    //--------------------------------------------------------------------------
    void AssetBrowser::Refresh(const QString& path)
    {
      foundation::Path full_path = path.toStdString().c_str();

      foundation::Vector<engine::AssetService::AssetFile> assets = 
        engine::AssetService::EnumerateAssets(
          full_path, 
          full_path, 
          false, 
          true);

      
      std::sort(assets.begin(), assets.end(), [](
        const engine::AssetService::AssetFile& a, 
        const engine::AssetService::AssetFile& b)
      {
        return a.type > b.type;
      });

      int row = 0;

      for (size_t i = 0; i < assets.size(); ++i)
      {

        const engine::AssetService::AssetFile& asset = assets.at(i);

        AssetBrowserItem* item = new AssetBrowserItem(
          asset.type, 
          asset.relative_path.ToString().c_str(), 
          this);

        int column = static_cast<int>(i) % kItemsPerRow_;
        asset_list_->addWidget(item, row, column);
        asset_list_->setColumnMinimumWidth(column, 128);

        if (column == kItemsPerRow_ - 1)
        {
          asset_list_->setColumnStretch(column, 1);
          ++row;
        }
      }
    }
  }
}