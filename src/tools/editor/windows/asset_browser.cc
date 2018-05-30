#include "tools/editor/windows/asset_browser.h"
#include "tools/editor/definitions/editor_colors.h"

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
      tree_(tree),
      assets_(assets),
      model_(nullptr)
    {
      ApplyStyle();
      TestAssets();
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
    }

    //--------------------------------------------------------------------------
    void AssetBrowser::TestAssets()
    {
      //!< @todo Remove this function when I've implemented showing assets

      const int num_rows = 2;

      for (int r = 0; r < num_rows; ++r)
      {
        for (int c = 0; c < kItemsPerRow_; ++c)
        {
          assets_->addWidget(new AssetBrowserItem("test_item"), r, c);
        }
      }

      assets_->setRowStretch(num_rows - 1, 1);
    }

    //--------------------------------------------------------------------------
    void AssetBrowser::ApplyStyle()
    {
      assets_->setMargin(kContentMargin_);
    }
  }
}