#include "tools/editor/asset-browser/asset_browser.h"
#include "tools/editor/asset-browser/asset_browser_item.h"
#include "tools/editor/asset-browser/asset_tree.h"
#include "tools/editor/custom-layouts/flowlayout.h"
#include "tools/compilers/definitions/default_assets.h"

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
    struct CreateAssetDesc
    {
      const char* name;
      compilers::AssetTypes type;
    };

    //--------------------------------------------------------------------------
    const CreateAssetDesc kCreateAssetDescs[] =
    {
      { "Scene", compilers::AssetTypes::kScene },
      { nullptr },
      { "Script", compilers::AssetTypes::kScript },
      { nullptr },
      { "Vertex shader", compilers::AssetTypes::kVertexShader },
      { "Pixel shader", compilers::AssetTypes::kPixelShader },
      { "Geometry shader", compilers::AssetTypes::kGeometryShader }
    };

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
      QString format;

      if (is_file == false)
      {
        format = file_or_dir + "%0";
      }
      else
      {
        foundation::Path no_ext = file_or_dir.toLatin1().data();
        foundation::String ext = no_ext.extension();

        no_ext = no_ext.NoExtension();

        format = QString(no_ext.ToString().c_str()) + "%0." + ext.c_str();
      }

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
        bool is_same = a.type == b.type;

        if (is_same == false)
        {
          return a.type > b.type;
        }

        const foundation::String& sa = a.relative_path.ToString();
        const foundation::String& sb = b.relative_path.ToString();

        return sa > sb && sa.size() < sb.size();
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
    void AssetBrowser::OnItemSelect(AssetBrowserItem* item)
    {
      selected_item_ = item;

      emit SelectionChanged(item);
    }

    //--------------------------------------------------------------------------
    void AssetBrowser::OnItemHovered(AssetBrowserItem* item, bool hovered)
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
    void AddToContextMenu(
      AssetBrowser* browser,
      QMenu* menu, 
      QAction* action, 
      const foundation::Function<void()>& func)
    {
      menu->addAction(action);
      QObject::connect(
        action,
        &QAction::triggered,
        browser,
        func);
    }

    //--------------------------------------------------------------------------
    void AssetBrowser::ShowItemContextMenu(QMenu* menu)
    {
      using AB = AssetBrowser;

      QAction* del = new QAction("Delete");
      AddToContextMenu(this, menu, del, [this]()
      { 
        DeleteHoveredItem(); 
      });

      QAction* rename = new QAction("Rename");
      AddToContextMenu(this, menu, rename, [this]()
      {
        RenameHoveredItem();
      });
    }

    //--------------------------------------------------------------------------
    void AssetBrowser::ShowRegularContextMenu(QMenu* menu)
    {
      using AB = AssetBrowser;

      QAction* create_dir = new QAction("Create directory");
      AddToContextMenu(this, menu, create_dir, [this]()
      { 
        CreateNewSourceDirectory(kNewDirectoryName_);
      });

      menu->addSeparator();

      QMenu* create_sub_menu = menu->addMenu("Create asset");

      int count = sizeof(kCreateAssetDescs) / sizeof(CreateAssetDesc);

      for (int i = 0; i < count; ++i)
      {
        const CreateAssetDesc& desc = kCreateAssetDescs[i];

        if (desc.name == nullptr)
        {
          create_sub_menu->addSeparator();
          continue;
        }

        QAction* create_action = new QAction(desc.name);
        AddToContextMenu(this, create_sub_menu, create_action, [this, desc]()
        {
          QString fixed_name = desc.name;
          fixed_name = fixed_name.replace(" ", "-").toLower();
          fixed_name = "new-" + fixed_name;

          CreateNewAsset(desc.type, fixed_name);
        });
      }
    }

    //--------------------------------------------------------------------------
    void AssetBrowser::CreateNewSourceDirectory(const QString& name) const
    {
      QString base_dir = GetCurrentSourceDirectory(navigation_path_);
      QString current_dir = GetUniqueFileOrDirectoryName(
        base_dir, name, false);

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
    void AssetBrowser::CreateNewAsset(
      compilers::AssetTypes type,
      const QString& new_name) const
    {
      const char* ext = compilers::AssetTypesToSourceExtension(type);

      QString fixed_name = new_name + '.' + ext;

      QString base_dir = GetCurrentSourceDirectory(navigation_path_);
      QString current_file = GetUniqueFileOrDirectoryName(
        base_dir, fixed_name, true);

      foundation::Path new_file_path = current_file.toLatin1().data();

      foundation::File new_file;
      new_file.Open(new_file_path, foundation::FileFlags::kWrite);

      if (new_file.is_ok() == false)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kEditor,
          foundation::LogSeverity::kError,
          "Could not create new asset '{0}',"
          "do you have the right permissions?",
          new_file_path);

        return;
      }

      using AST = compilers::AssetTypes;

      const int asset_count = static_cast<int>(AST::kCount);
      const char* default_assets[asset_count];
      memset(default_assets, 0, sizeof(const char*) * asset_count);

      default_assets[static_cast<int>(AST::kVertexShader)] =
        compilers::kDefaultVSShader;

      default_assets[static_cast<int>(AST::kPixelShader)] =
        compilers::kDefaultPSShader;

      default_assets[static_cast<int>(AST::kGeometryShader)] =
        compilers::kDefaultGSShader;

      const char* default_contents = default_assets[static_cast<int>(type)];
      if (default_contents != nullptr)
      {
        new_file.Write(
          reinterpret_cast<const uint8_t*>(default_contents),
          strlen(default_contents));
      }

      new_file.Close();
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

      bool is_dir = full_path.is_directory();
      foundation::Path file_or_dir = 
        full_path.StripPath(base_dir).NoExtension();

      QString src_dir = GetCurrentSourceDirectory(base_dir.ToString().c_str());
      foundation::Path to_delete = src_dir.toLatin1().data();

      if (is_dir == false)
      {
        file_or_dir += ".";
        file_or_dir +=
          compilers::AssetTypesToSourceExtension(last_hovered_item_->type());
      }

      to_delete /= file_or_dir;

      QMessageBox::StandardButton reply =
        QMessageBox::question(
          nullptr, 
          "Confirmation", 
          QString("Are you sure you want to delete '%0'? This cannot be undone")
            .arg(file_or_dir.NoExtension().ToString().c_str()),
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

      last_hovered_item_ = nullptr;
      hovered_item_ = nullptr;
    }

    //--------------------------------------------------------------------------
    void AssetBrowser::RenameHoveredItem()
    {
      if (last_hovered_item_ == nullptr)
      {
        return;
      }

      bool was_changed = false;
      QString old_name;
      QString new_name = last_hovered_item_->Rename(&was_changed, &old_name);

      if (was_changed == false)
      {
        return;
      }

      foundation::Path full_path = 
        last_hovered_item_->full_path().toLatin1().data();

      compilers::AssetTypes type = last_hovered_item_->type();

      bool is_dir = full_path.is_directory();
      if (is_dir == true)
      {
        CreateNewSourceDirectory(new_name);
      }
      else
      {
        CreateNewAsset(type, new_name.toLatin1().data());
      }

      QString base_dir = GetCurrentSourceDirectory(navigation_path_);
      base_dir = base_dir + '/' + old_name;

      if (is_dir == false)
      {
        base_dir = 
          base_dir + '.' + compilers::AssetTypesToSourceExtension(type);

        foundation::File::Remove(base_dir.toLatin1().data());
      }
      else
      {
        foundation::Directory::Remove(base_dir.toLatin1().data());
      }

      delete last_hovered_item_;

      last_hovered_item_ = nullptr;
      hovered_item_ = nullptr;
    }
  }
}