#include "tools/editor/asset-browser/asset_browser.h"
#include "tools/editor/asset-browser/asset_browser_item.h"
#include "tools/editor/custom-layouts/flowlayout.h"

#include <engine/services/asset_service.h>

#include <QTreeView>
#include <QSplitter>
#include <QLayout>
#include <QScrollArea>
#include <QSettings>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    const int AssetBrowser::kFlowLayoutSpacing_ = 10;
    const QString AssetBrowser::kSettingsAssetBrowserKey_ 
      = "AssetBrowser.Splitter";

    //--------------------------------------------------------------------------
    AssetBrowser::AssetBrowser(QWidget* parent) :
      QWidget(parent),
      tree_(nullptr),
      asset_list_(nullptr),
      splitter_(nullptr)
    {
      setObjectName(QStringLiteral("AssetBrowser"));

      QHBoxLayout* main_layout = new QHBoxLayout();

      tree_ = new QTreeView(this);
      tree_->setObjectName(QStringLiteral("AssetBrowserTree"));

      QFrame* browser_frame = new QFrame(this);
      browser_frame->setObjectName(QStringLiteral("AssetBrowserFrame"));
      browser_frame->setFrameStyle(
        QFrame::Shape::StyledPanel | QFrame::Shadow::Sunken);

      const int s = kFlowLayoutSpacing_;
      const int scrollbar_width = 32;
      const int min_width = 
        AssetBrowserItem::kMaxItemSize.width() * 2 + s * 4 + scrollbar_width;

      asset_list_ = new FlowLayout(browser_frame, s, s, s);
      asset_list_->setObjectName(QStringLiteral("AssetBrowserListLayout"));

      browser_frame->setLayout(asset_list_);

      QScrollArea* scroll_area = new QScrollArea();
      scroll_area->setObjectName(QStringLiteral("AssetBrowserFrameScrollArea"));
      scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
      scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
      scroll_area->setWidgetResizable(true);
      scroll_area->setWidget(browser_frame);
      scroll_area->setMinimumWidth(min_width);

      splitter_ = new QSplitter(this);
      splitter_->setObjectName(QStringLiteral("AssetBrowserSplitter"));
      splitter_->setOrientation(Qt::Orientation::Horizontal);

      splitter_->addWidget(tree_);
      splitter_->addWidget(scroll_area);

      main_layout->addWidget(splitter_);
      
      setLayout(main_layout);
    }

    //--------------------------------------------------------------------------
    void AssetBrowser::SaveState(QSettings* settings)
    {
      if (splitter_ == nullptr)
      {
        return;
      }

      settings->setValue(kSettingsAssetBrowserKey_, splitter_->saveState());
    }

    //--------------------------------------------------------------------------
    void AssetBrowser::LoadState(const QSettings* settings)
    {
      if (splitter_ == nullptr)
      {
        return;
      }

      splitter_->restoreState(
        settings->value(kSettingsAssetBrowserKey_).toByteArray());
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

        asset_list_->addWidget(item);
      }
    }
  }
}