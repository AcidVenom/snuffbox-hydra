#include "tools/editor/asset-browser/asset_browser.h"
#include "tools/editor/asset-browser/asset_browser_item.h"
#include "tools/editor/asset-browser/asset_tree.h"
#include "tools/editor/custom-layouts/flowlayout.h"

#include <engine/services/asset_service.h>
#include <foundation/containers/function.h>

#include <QSplitter>
#include <QLayout>
#include <QScrollArea>
#include <QSettings>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    class ClickableFrame : public QFrame
    {

    public:

      ClickableFrame(
        const foundation::Function<void()>& callback, 
        QWidget* parent = nullptr) :
        QFrame(parent),
        callback_(callback)
      {

      }

    protected:

      void mouseReleaseEvent(QMouseEvent* evt) override
      {
        callback_();
        QFrame::mouseReleaseEvent(evt);
      }

    private:

      foundation::Function<void()> callback_;
    };

    //--------------------------------------------------------------------------
    const int AssetBrowser::kFlowLayoutSpacing_ = 10;
    const QString AssetBrowser::kSettingsAssetBrowserKey_ 
      = "AssetBrowser.Splitter";

    //--------------------------------------------------------------------------
    AssetBrowser::AssetBrowser(const QString& build_path, QWidget* parent) :
      QWidget(parent),
      build_path_(build_path),
      navigation_path_(build_path),
      tree_(nullptr),
      asset_list_(nullptr),
      splitter_(nullptr),
      selected_item_(nullptr)
    {
      setObjectName(QStringLiteral("AssetBrowser"));

      QHBoxLayout* main_layout = new QHBoxLayout();

      tree_ = new AssetTree(build_path_, this);

      ClickableFrame* browser_frame = new ClickableFrame([&]()
      {
        emit SelectionChanged(nullptr);
      }, this);

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

      connect(
        tree_,
        &AssetTree::DirectorySelected,
        this,
        &AssetBrowser::OnDirectorySelected);
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
    void AssetBrowser::RefreshBrowser()
    {
      QLayoutItem* old_item = nullptr;
      while ((old_item = asset_list_->takeAt(0)) != nullptr)
      {
        delete old_item->widget();
        delete old_item;
      }

      foundation::Path full_path = navigation_path_.toLatin1().data();

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
          full_path.ToString().c_str(),
          this);

        connect(
          item, 
          &AssetBrowserItem::DirectoryChanged, 
          this, 
          &AssetBrowser::OnDirectorySelected);

        asset_list_->addWidget(item);
      }
    }

    //--------------------------------------------------------------------------
    void AssetBrowser::Refresh()
    {
      RefreshBrowser();
    }

    //--------------------------------------------------------------------------
    void AssetBrowser::OnItemSelect(const AssetBrowserItem* item)
    {
      selected_item_ = item;

      emit SelectionChanged(item);
    }

    //--------------------------------------------------------------------------
    void AssetBrowser::OnDirectorySelected(const QString& directory)
    {
      navigation_path_ = directory;
      RefreshBrowser();
    }
  }
}