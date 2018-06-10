#include "tools/editor/windows/asset_browser.h"
#include "tools/editor/definitions/editor_colors.h"

#include <engine/services/asset_service.h>

#include <foundation/auxiliary/string_utils.h>

#include <qfilesystemmodel.h>
#include <qtreeview.h>
#include <qgridlayout.h>
#include <qlabel.h>
#include <qevent.h>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    const int AssetBrowserItem::kIconSize_ = 64;
    const int AssetBrowserItem::kLabelHeight_ = 20;

    const int AssetBrowser::kItemsPerRow_ = 3;
    const int AssetBrowser::kContentMargin_ = 10;

    //--------------------------------------------------------------------------
    AssetBrowserItem::AssetBrowserItem(
      AssetBrowser* browser,
      const QString& name,
      const foundation::Path& path,
      compilers::AssetTypes type)
      :
      browser_(browser),
      path_(path),
      type_(type),
      icon_(nullptr),
      label_(nullptr)
    {
      CreateItem(name);
      SetSelected(false);
    }

    //--------------------------------------------------------------------------
    void AssetBrowserItem::CreateItem(const QString& name)
    {
      QWidget* icon_wrapper = new QWidget();
      QVBoxLayout* wrapper_layout = new QVBoxLayout();

      icon_wrapper->setLayout(wrapper_layout);

      icon_ = new QFrame();
      icon_->setSizePolicy({ QSizePolicy::Fixed, QSizePolicy::Fixed });
      icon_->setFixedSize(QSize{ kIconSize_, kIconSize_ });

      wrapper_layout->setAlignment(Qt::AlignHCenter);
      wrapper_layout->addWidget(icon_);
      wrapper_layout->addStretch();

      label_ = new QLabel();
      label_->setText(name);
      label_->setFixedSize(QSize{ kIconSize_ * 2, kLabelHeight_ });
      label_->setSizePolicy({ QSizePolicy::Fixed, QSizePolicy::Fixed });
      label_->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

      QVBoxLayout* widget_layout = new QVBoxLayout(this);
      widget_layout->setAlignment(Qt::AlignHCenter);

      widget_layout->addWidget(icon_wrapper);
      widget_layout->addWidget(label_);
      widget_layout->addStretch();

      setLayout(widget_layout);

      connect(
        browser_,
        SIGNAL(DeselectAll()),
        this,
        SLOT(Deselect()));
    }

    //--------------------------------------------------------------------------
    void AssetBrowserItem::SetSelected(bool selected)
    {
      QPalette::ColorRole role = 
        selected == true ? QPalette::Highlight : QPalette::Base;

      QString icon_col = EditorColors::ColorToCSS(
        EditorColors::DefaultPalette().color(role));

      QString label_col = 
        selected == true ? 
        icon_col : EditorColors::ColorToCSS(EditorColors::DockColor());

      icon_->setStyleSheet(
        "QFrame{ background-color: " + icon_col + "; border-radius: 4px; }");

      label_->setStyleSheet(
        "QLabel{ background-color: " + label_col + "; border-radius: 4px; }");
    }

    //--------------------------------------------------------------------------
    void AssetBrowserItem::mouseDoubleClickEvent(QMouseEvent* e)
    {
      if (e->button() == Qt::LeftButton)
      {
        emit browser_->DoubleClickedAsset(
          path_.ToString().c_str(), 
          static_cast<int>(type_));
      }  
    }

    //--------------------------------------------------------------------------
    void AssetBrowserItem::mousePressEvent(QMouseEvent* e)
    {
      if (e->button() == Qt::LeftButton)
      {
        emit browser_->DeselectAll();
        SetSelected(true);
      }
    }

    //--------------------------------------------------------------------------
    void AssetBrowserItem::Deselect()
    {
      SetSelected(false);
    }

    //--------------------------------------------------------------------------
    AssetBrowser::AssetBrowser(QTreeView* tree, QGridLayout* assets) :
      root_(""),
      tree_(tree),
      assets_(assets),
      model_(nullptr),
      current_row_(0),
      current_column_(0)
    {
      ApplyStyle();
      BindEvents();
    }

    //--------------------------------------------------------------------------
    void AssetBrowser::Refresh(const foundation::Path& path)
    {
      if (model_ != nullptr)
      {
        delete model_;
        model_ = nullptr;
      }

      model_ = new QFileSystemModel(this);

      model_->setReadOnly(true);
      model_->setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

      QString root = path.ToString().c_str();
      model_->setRootPath(root);

      tree_->setModel(model_);
      tree_->setRootIndex(model_->index(root));

      root_ = root;

      Clear();
    }

    //--------------------------------------------------------------------------
    void AssetBrowser::ShowDirectory(const foundation::Path& path)
    {
      Clear();

      foundation::Path asset_dir = (root_ + "/assets").toStdString().c_str();

      const foundation::Vector<engine::AssetService::AssetFile>& paths =
        engine::AssetService::EnumerateAssets(path, asset_dir);

      QString full_path;
      for (size_t i = 0; i < paths.size(); ++i)
      {
        const engine::AssetService::AssetFile& file = paths.at(i);
        full_path = root_ + "/" + file.relative_path.ToString().c_str();

        AddFromPath(full_path.toStdString().c_str(), file.type);
      }

      assets_->setRowStretch(current_row_ + 1, 1);

      if (current_row_ == 0 && current_column_ < kItemsPerRow_)
      {
        assets_->setColumnStretch(current_column_ + 1, 1);
      }
    }

    //--------------------------------------------------------------------------
    void AssetBrowser::AddFromPath(
      const foundation::Path& path,
      compilers::AssetTypes type)
    {
      foundation::Path relative = path.StripPath(root_.toStdString().c_str());
      foundation::String name = relative.NoExtension().ToString();

      foundation::Vector<foundation::String> split = 
        foundation::StringUtils::Split(name, '/');

      name = split.at(split.size() - 1);

      AddWidget(
        new AssetBrowserItem(this, name.c_str(), relative, type));
    }

    //--------------------------------------------------------------------------
    void AssetBrowser::AddWidget(QWidget* widget)
    {
      assets_->addWidget(widget, current_row_, current_column_);
      ++current_column_;

      if (current_column_ == kItemsPerRow_)
      {
        current_column_ = 0;
        ++current_row_;
      }
    }

    //--------------------------------------------------------------------------
    void AssetBrowser::Clear()
    {
      assets_->setColumnStretch(current_column_ + 1, 0);
      assets_->setRowStretch(current_row_ + 1, 0);

      current_row_ = current_column_ = 0;

      QLayoutItem* item;
      while ((item = assets_->takeAt(0)) != nullptr)
      {
        delete item->widget();
        delete item;
      }
    }

    //--------------------------------------------------------------------------
    void AssetBrowser::ApplyStyle()
    {
      assets_->setMargin(kContentMargin_);
    }

    //--------------------------------------------------------------------------
    void AssetBrowser::BindEvents()
    {
      connect(
        tree_,
        &QTreeView::clicked,
        this,
        &AssetBrowser::OnSelectionChanged);
    }

    //--------------------------------------------------------------------------
    void AssetBrowser::OnSelectionChanged(const QModelIndex& index)
    {
      std::string path = model_->filePath(index).toStdString();
      foundation::Path to_traverse = path.c_str();

      ShowDirectory(to_traverse);
    }
  }
}