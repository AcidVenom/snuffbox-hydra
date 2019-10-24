#include "tools/editor/asset-browser/asset_browser.h"
#include "tools/editor/asset-browser/asset_browser_item.h"
#include "tools/editor/asset-browser/asset_tree.h"
#include "tools/editor/custom-layouts/flowlayout.h"

#include <engine/services/asset_service.h>
#include <foundation/containers/function.h>
#include <foundation/io/directory.h>
#include <foundation/io/file.h>

#include <QSplitter>
#include <QLayout>
#include <QScrollArea>
#include <QSettings>
#include <QMouseEvent>
#include <QMenu>
#include <QMessageBox>

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
        Qt::MouseButton button = evt->button();
        if (button == Qt::MouseButton::LeftButton)
        {
          callback_();
        }

        QFrame::mouseReleaseEvent(evt);
      }

    private:

      foundation::Function<void()> callback_;
    };

    //--------------------------------------------------------------------------
    const int AssetBrowser::kFlowLayoutSpacing_ = 10;
    const QString AssetBrowser::kSettingsAssetBrowserKey_ 
      = "AssetBrowser.Splitter";

    const QString AssetBrowser::kNewDirectoryName_ = "new-directory";

    //--------------------------------------------------------------------------
    AssetBrowser::AssetBrowser(
      const QString& source_path,
      const QString& build_path, 
      QWidget* parent) :
      QWidget(parent),
      source_path_(source_path),
      build_path_(build_path),
      navigation_path_(build_path),
      tree_(nullptr),
      asset_list_(nullptr),
      splitter_(nullptr),
      browser_frame_(nullptr),
      selected_item_(nullptr),
      hovered_item_(nullptr),
      last_hovered_item_(nullptr)
    {
      setObjectName(QStringLiteral("AssetBrowser"));

      QHBoxLayout* main_layout = new QHBoxLayout();

      tree_ = new AssetTree(build_path_, this);

      ClickableFrame* browser_frame = new ClickableFrame([&]()
      {
        emit SelectionChanged(nullptr);
      }, this);

      browser_frame->setContextMenuPolicy(
        Qt::ContextMenuPolicy::CustomContextMenu);

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

      browser_frame_ = browser_frame;

      connect(
        browser_frame,
        &QFrame::customContextMenuRequested,
        this,
        &AssetBrowser::CustomContextMenu);
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
    QString AssetBrowser::GetCurrentSourceDirectory(const QString& from) const
    {
      foundation::Path relative_dir = from.toLatin1().data();
      relative_dir = relative_dir.StripPath(build_path_.toLatin1().data());

      foundation::Path source_dir = source_path_.toLatin1().data();
      source_dir /= relative_dir;

      return source_dir.ToString().c_str();
    }

    //--------------------------------------------------------------------------
    QString AssetBrowser::GetUniqueFileOrDirectoryName(
      const QString& base_dir,
      const QString& file_or_dir,
      bool is_file)
    {
      QString format = file_or_dir + "%0";
      QString current_dir = base_dir + '/' + format.arg("");

      int copy_count = 1;

      bool(*Checker)(const foundation::Path&) = 
        is_file ? foundation::File::Exists : foundation::Directory::Exists;

      while (Checker(current_dir.toLatin1().data()))
      {
        QString post_fix = QString("-%0").arg(copy_count);
        current_dir = base_dir + '/' + format.arg(post_fix);

        ++copy_count;
      }

      return current_dir;
    }

    //--------------------------------------------------------------------------
    void AssetBrowser::RefreshBrowser()
    {
      selected_item_ = nullptr;

      last_hovered_item_ = hovered_item_;
      hovered_item_ = nullptr;

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

        connect(
          item,
          &AssetBrowserItem::Hovered,
          this,
          &AssetBrowser::OnItemHovered);

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
    void AssetBrowser::OnItemHovered(const AssetBrowserItem* item, bool hovered)
    {
      last_hovered_item_ = hovered_item_;

      if (hovered == true)
      {
        hovered_item_ = item;
      }
      else
      {
        if (hovered_item_ == item)
        {
          hovered_item_ = nullptr;
        }
      }
    }

    //--------------------------------------------------------------------------
    void AssetBrowser::OnDirectorySelected(const QString& directory)
    {
      navigation_path_ = directory;
      RefreshBrowser();
    }

    //--------------------------------------------------------------------------
    void AssetBrowser::CustomContextMenu(const QPoint& pos)
    {
      QMenu ctx("Actions", this);

      if (hovered_item_ != nullptr)
      {
        ShowItemContextMenu(&ctx);
      }
      else
      {
        ShowRegularContextMenu(&ctx);
      }

      ctx.exec(browser_frame_->mapToGlobal(pos));
    }
    //--------------------------------------------------------------------------

#define DECL_ACTION_CB(ctx, name, cb)\
connect(## name ##, &QAction::triggered, this, &AssetBrowser::##cb##);\
ctx->addAction(## name ##);

    //--------------------------------------------------------------------------
    void AssetBrowser::ShowItemContextMenu(QMenu* menu)
    {
      QAction* delete_action = new QAction("Delete");
      DECL_ACTION_CB(menu, delete_action, DeleteHoveredItem);
    }

    //--------------------------------------------------------------------------
    void AssetBrowser::ShowRegularContextMenu(QMenu* menu)
    {
      QAction* create_dir_action = new QAction("Create directory");
      DECL_ACTION_CB(menu, create_dir_action, CreateNewSourceDirectory);
    }

    //--------------------------------------------------------------------------
    void AssetBrowser::CreateNewSourceDirectory()
    {
      QString base_dir = GetCurrentSourceDirectory(navigation_path_);
      QString current_dir = GetUniqueFileOrDirectoryName(
        base_dir, kNewDirectoryName_, false);

      foundation::Path new_dir_path = current_dir.toLatin1().data();

      foundation::Directory new_dir;
      new_dir.Open(new_dir_path);

      if (new_dir.is_ok() == false)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kEditor,
          foundation::LogSeverity::kError,
          "Could not create new directory '{0}',"
          "do you have the right permissions?",
          new_dir_path);
      }
    }

    //--------------------------------------------------------------------------
    void AssetBrowser::DeleteHoveredItem()
    {
      if (last_hovered_item_ == nullptr)
      {
        return;
      }

      foundation::Path full_path = 
        last_hovered_item_->full_path().toLatin1().data();

      foundation::Path base_dir = full_path;
      base_dir = base_dir.GetBaseDirectory();

      foundation::Path file_or_dir = full_path.StripPath(base_dir);

      QString src_dir = GetCurrentSourceDirectory(base_dir.ToString().c_str());
      foundation::Path to_delete = src_dir.toLatin1().data();
      to_delete /= file_or_dir;

      QMessageBox::StandardButton reply =
        QMessageBox::question(
          nullptr, 
          "Confirmation", 
          QString("Are you sure you want to delete '%0', this cannot be undone")
            .arg(file_or_dir.ToString().c_str()),
          QMessageBox::Yes | QMessageBox::No);

      if (reply == QMessageBox::No)
      {
        return;
      }

      if (foundation::File::Exists(to_delete) == true)
      {
        foundation::File::Remove(to_delete);
      }
      else if (foundation::Directory::Exists(to_delete) == true)
      {
        foundation::Directory::Remove(to_delete);
      }

      delete last_hovered_item_;

      last_hovered_item_ = hovered_item_;
      hovered_item_ = nullptr;
    }
  }
}