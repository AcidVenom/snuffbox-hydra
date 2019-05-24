#include "tools/editor/asset-browser/asset_icon.h"

#include <foundation/io/resources.h>
#include <foundation/io/file.h>
#include <foundation/io/path.h>

#include <QMap>
#include <QImage>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <resources/directory_icon.h>
#include <resources/scene_icon.h>
#include <resources/script_icon.h>
#include <resources/material_icon.h>
#include <resources/model_icon.h>
#include <resources/vertex_shader_icon.h>
#include <resources/pixel_shader_icon.h>
#include <resources/geometry_shader_icon.h>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    const int AssetIcon::kDefaultIconSize_ = 48;

    //--------------------------------------------------------------------------
    AssetIcon::AssetIcon(
      compilers::AssetTypes type, 
      const QString& path, 
      QWidget* parent) 
      :
      QWidget(parent),
      type_(type),
      path_(path),
      icon_size_(kDefaultIconSize_),
      icon_label_(nullptr)
    {
      setObjectName(QStringLiteral("AssetIcon"));

      QVBoxLayout* layout = new QVBoxLayout(this);
      layout->setObjectName(QStringLiteral("AssetIconLayout"));

      icon_label_ = new QLabel(this);
      icon_label_->setObjectName(QStringLiteral("AssetIconIconLabel"));

      SetType(type);
      SetIconSize(kDefaultIconSize_);

      name_label_ = new QLabel(this);
      name_label_->setObjectName(QStringLiteral("AssetIconNameLabel"));

      SetNameFromPath(path);

      layout->addWidget(icon_label_);
      layout->addWidget(name_label_);

      icon_label_->setAlignment(Qt::AlignCenter);
      name_label_->setAlignment(Qt::AlignCenter);

      setLayout(layout);
    }

    //--------------------------------------------------------------------------
    void AssetIcon::SetType(compilers::AssetTypes type)
    {
      type_ = type;

      const QImage* img = AssetTypesToIcon(type_);
      QPixmap pixmap = QPixmap::fromImage(*img);
 
      QSize size = QSize(kDefaultIconSize_, kDefaultIconSize_);
      icon_label_->setPixmap(
        pixmap.scaled(
          size,
          Qt::AspectRatioMode::IgnoreAspectRatio, 
          Qt::TransformationMode::SmoothTransformation));
    }

    //--------------------------------------------------------------------------
    void AssetIcon::SetNameFromPath(const QString& path)
    {
      foundation::Path full_path = foundation::Path(path.toStdString().c_str());
      full_path = full_path.NoExtension();

      foundation::Vector<foundation::String> split = 
        foundation::StringUtils::Split(full_path.ToString(), '/');

      foundation::String name = "";

      if (split.size() > 0)
      {
        name = split.at(split.size() - 1);
      }

      name_label_->setText(name.c_str());
    }

    //--------------------------------------------------------------------------
    compilers::AssetTypes AssetIcon::type() const
    {
      return type_;
    }

    //--------------------------------------------------------------------------
    const QString& AssetIcon::path() const
    {
      return path_;
    }

    //--------------------------------------------------------------------------
    void AssetIcon::SetIconSize(int size)
    {
      icon_size_ = size;
      icon_label_->setFixedSize(icon_size_, icon_size_);
    }

    //--------------------------------------------------------------------------
    int AssetIcon::icon_size() const
    {
      return icon_size_;
    }

    //--------------------------------------------------------------------------
    void AssetIcon::LoadAllIcons()
    {
      static bool initialized = false;

      if (initialized == false)
      {
        SNUFF_MAKE_RESOURCE(directory_icon);
        SNUFF_MAKE_RESOURCE(scene_icon);
        SNUFF_MAKE_RESOURCE(script_icon);
        SNUFF_MAKE_RESOURCE(material_icon);
        SNUFF_MAKE_RESOURCE(model_icon);
        SNUFF_MAKE_RESOURCE(vertex_shader_icon);
        SNUFF_MAKE_RESOURCE(pixel_shader_icon);
        SNUFF_MAKE_RESOURCE(geometry_shader_icon);

        initialized = true;
      }
    }

    //--------------------------------------------------------------------------
    const QImage* AssetIcon::AssetTypesToIcon(compilers::AssetTypes type)
    {
      LoadAllIcons();

      foundation::Path path = "snuff:/rsc/icons/";

      switch (type)
      {
      case compilers::AssetTypes::kCount:
        path /= "directory_icon";
        break;

      case compilers::AssetTypes::kScene:
        path /= "scene_icon";
        break;

      case compilers::AssetTypes::kScript:
        path /= "script_icon";
        break;

      case compilers::AssetTypes::kMaterial:
        path /= "material_icon";
        break;

      case compilers::AssetTypes::kModel:
        path /= "model_icon";
        break;

      case compilers::AssetTypes::kVertexShader:
        path /= "vertex_shader_icon";
        break;

      case compilers::AssetTypes::kPixelShader:
        path /= "pixel_shader_icon";
        break;

      case compilers::AssetTypes::kGeometryShader:
        path /= "geometry_shader_icon";
        break;
      }

      path += ".png";

      QString qpath = path.ToString().c_str();
      static QMap<QString, QImage> asset_icons;

      QMap<QString, QImage>::const_iterator it = asset_icons.find(qpath);
      if (it != asset_icons.end())
      {
        return &(*it);
      }

      foundation::File fin(path);
      if (fin.is_ok() == false)
      {
        Q_ASSERT(false && "Could not open icon from virtual file system");
        return nullptr;
      }

      size_t length;
      const uint8_t* buffer = fin.ReadBuffer(&length);

      int size = static_cast<int>(length);
      return &(*asset_icons.insert(qpath, QImage::fromData(buffer, size)));
    }
  }
}