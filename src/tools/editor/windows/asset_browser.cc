#include "tools/editor/windows/asset_browser.h"
#include "tools/editor/definitions/editor_colors.h"
#include "tools/editor/definitions/project.h"

#include <foundation/auxiliary/string_utils.h>
#include <foundation/io/resources.h>
#include <foundation/io/file.h>

#include <qfilesystemmodel.h>
#include <qtreeview.h>
#include <qgridlayout.h>
#include <qlabel.h>
#include <qevent.h>

#include <EASTL/sort.h>

#include <resources/directory_icon.h>
#include <resources/script_icon.h>
#include <resources/scene_icon.h>
#include <resources/vertex_shader_icon.h>
#include <resources/pixel_shader_icon.h>
#include <resources/geometry_shader_icon.h>
#include <resources/material_icon.h>

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
      const foundation::Path& relative,
      const foundation::Path& full_path,
      compilers::AssetTypes type)
      :
      browser_(browser),
      relative_(relative),
      full_path_(full_path),
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
      QVBoxLayout* icon_layout = new QVBoxLayout();

      icon_wrapper->setLayout(wrapper_layout);

      icon_ = new QFrame();
      icon_->setSizePolicy({ QSizePolicy::Fixed, QSizePolicy::Fixed });
      icon_->setFixedSize(QSize{ kIconSize_, kIconSize_ });
      icon_->setContentsMargins(0, 0, 0, 0);

      QLabel* icon_img = new QLabel();
      icon_img->setSizePolicy({ QSizePolicy::Fixed, QSizePolicy::Fixed });
      icon_img->setFixedSize(QSize{ kIconSize_, kIconSize_ });

      QImage* img = browser_->GetIcon(type_);
      if (img != nullptr)
      {
        icon_img->setPixmap(QPixmap::fromImage(*img));
      }

      icon_img->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

      icon_layout->addWidget(icon_img);
      icon_layout->setContentsMargins(0, 0, 0, 0);
      icon_layout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

      icon_->setLayout(icon_layout);

      wrapper_layout->setAlignment(Qt::AlignHCenter);
      wrapper_layout->addWidget(icon_);

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
        selected == true ? QPalette::Highlight : QPalette::Window;

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
        if (type_ != compilers::AssetTypes::kCount)
        {
          emit browser_->DoubleClickedAsset(
            relative_.ToString().c_str(), 
            static_cast<int>(type_));

          return;
        }

        foundation::Path copy = full_path_;
        AssetBrowser* browser = browser_;

        browser->ShowDirectory(copy);
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
      LoadIcons();
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
    QImage* AssetBrowser::GetIcon(compilers::AssetTypes type)
    {
      return icons_[static_cast<int>(type)];
    }

    //--------------------------------------------------------------------------
    void AssetBrowser::ShowDirectory(const foundation::Path& path)
    {
      Clear();

      foundation::Path asset_dir = 
        (root_ + "/" + Project::kAssetFolder).toStdString().c_str();

      foundation::Vector<engine::AssetService::AssetFile> paths =
        engine::AssetService::EnumerateAssets(path, asset_dir, false, true);

      eastl::sort(paths.begin(), paths.end(), AssetSorter());

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
      std::string s = root_.toStdString();
      foundation::Path root = s.c_str();

      foundation::Path relative = path.StripPath(root);
      foundation::Path full = root / Project::kAssetFolder / relative;

      foundation::String name = relative.NoExtension().ToString();

      foundation::Vector<foundation::String> split = 
        foundation::StringUtils::Split(name, '/');

      name = split.at(split.size() - 1);

      AddWidget(
        new AssetBrowserItem(this, name.c_str(), relative, full, type));
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
    void AssetBrowser::LoadIcons()
    {
      SNUFF_MAKE_RESOURCE(directory_icon);
      SNUFF_MAKE_RESOURCE(script_icon);
      SNUFF_MAKE_RESOURCE(scene_icon);
      SNUFF_MAKE_RESOURCE(vertex_shader_icon);
      SNUFF_MAKE_RESOURCE(pixel_shader_icon);
      SNUFF_MAKE_RESOURCE(geometry_shader_icon);
      SNUFF_MAKE_RESOURCE(material_icon);

      CreateIcons();
    }

    //--------------------------------------------------------------------------
    void AssetBrowser::CreateIcons()
    {
      foundation::Path paths[] =
      {
        "snuff:/rsc/icons/script_icon.png",
        "snuff:/rsc/icons/scene_icon.png",
        "snuff:/rsc/icons/vertex_shader_icon.png",
        "snuff:/rsc/icons/pixel_shader_icon.png",
        "snuff:/rsc/icons/geometry_shader_icon.png",
        "snuff:/rsc/icons/material_icon.png",
        "snuff:/rsc/icons/directory_icon.png"
      };

      int path_len = static_cast<int>(sizeof(paths) / sizeof(foundation::Path));

      QImage* img = nullptr;
      foundation::File fin;

      size_t len;
      const uint8_t* data;

      for (int i = 0; i <= static_cast<int>(compilers::AssetTypes::kCount); ++i)
      {
        if (i >= path_len)
        {
          icons_[i] = nullptr;
          continue;
        }

        fin.Open(paths[i]);

        if (fin.is_ok() == false)
        {
          icons_[i] = nullptr;
          continue;
        }

        data = fin.ReadBuffer(&len);

        img = new QImage();
        img->loadFromData(data, static_cast<int>(len), "PNG");

        icons_[i] = img;
      }
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

    //--------------------------------------------------------------------------
    AssetBrowser::~AssetBrowser()
    {
      QImage* ptr = nullptr;
      for (int i = 0; i <= static_cast<int>(compilers::AssetTypes::kCount); ++i)
      {
         ptr = icons_[i];

         if (ptr != nullptr)
         {
           delete ptr;
         }
      }
    }

    //--------------------------------------------------------------------------
    bool AssetBrowser::AssetSorter::operator()(
      const engine::AssetService::AssetFile& a, 
      const engine::AssetService::AssetFile& b)
    {
      foundation::String as = a.relative_path.ToString();
      foundation::String bs = b.relative_path.ToString();

      if (a.is_directory == true && b.is_directory == false)
      {
        return true;
      }

      if (a.is_directory == false && b.is_directory == true)
      {
        return false;
      }

      if (a.type < b.type)
      {
        return true;
      }

      if (a.type > b.type)
      {
        return false;
      }

      return as < bs;
    }
  }
}