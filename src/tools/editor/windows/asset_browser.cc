#include "tools/editor/windows/asset_browser.h"
#include "tools/editor/definitions/editor_colors.h"

#include <engine/services/asset_service.h>

#include <foundation/auxiliary/string_utils.h>

#include <qfilesystemmodel.h>
#include <qtreeview.h>
#include <qgridlayout.h>
#include <qlabel.h>

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
    AssetBrowserItem::AssetBrowserItem(const QString& name) :
      icon_(nullptr),
      label_(nullptr)
    {
      CreateItem(name);
      ApplyStyle();
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
    }

    //--------------------------------------------------------------------------
    void AssetBrowserItem::ApplyStyle()
    {
      QString bg_col = EditorColors::ColorToCSS(
        EditorColors::DefaultPalette().color(QPalette::Base));

      icon_->setStyleSheet(
        "QFrame{ background-color: " + bg_col + "; border-radius: 4px; }");

      bg_col = EditorColors::ColorToCSS(EditorColors::DockColor());
      label_->setStyleSheet(
        "QLabel{ background-color: " + bg_col + "; border-radius: 4px; }");
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

      const foundation::Vector<engine::AssetService::AssetFile>& paths =
        engine::AssetService::EnumerateAssets(path);

      QString full_path;
      for (size_t i = 0; i < paths.size(); ++i)
      {
        const engine::AssetService::AssetFile& file = paths.at(i);
        full_path = root_ + "/" + file.relative_path.ToString().c_str();
        AddFromPath(full_path.toStdString().c_str());
      }

      assets_->setRowStretch(current_row_ + 1, 1);

      if (current_row_ == 0 && current_column_ < kItemsPerRow_)
      {
        assets_->setColumnStretch(current_column_ + 1, 1);
      }
    }

    //--------------------------------------------------------------------------
    void AssetBrowser::AddFromPath(const foundation::Path& path)
    {
      foundation::Path stripped = path.NoExtension();

      foundation::Vector<foundation::String> split = 
        foundation::StringUtils::Split(stripped.ToString(), '/');

      if (split.size() == 0)
      {
        return;
      }

      const foundation::String& name = split.at(split.size() - 1);
      AddWidget(new AssetBrowserItem(name.c_str()));
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