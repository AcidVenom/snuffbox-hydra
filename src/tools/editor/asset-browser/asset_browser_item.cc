#include "tools/editor/asset-browser/asset_browser_item.h"

#include "tools/editor/asset-browser/asset_icon.h"
#include "tools/editor/asset-browser/asset_browser.h"
#include "tools/editor/application/styling.h"

#include <QVBoxLayout>
#include <QStyle>
#include <QPainter>
#include <QStyleOption>
#include <QInputDialog>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    const QSize AssetBrowserItem::kMaxItemSize = QSize(96, 96);

    //--------------------------------------------------------------------------
    AssetBrowserItem::AssetBrowserItem(
      compilers::AssetTypes type,
      const QString& path,
      const QString& base_dir,
      AssetBrowser* parent)
      :
      QWidget(parent),
      type_(type),
      relative_path_(path),
      full_path_(base_dir + '/' + path),
      icon_(nullptr),
      selected_(false)
    {
      setObjectName(QStringLiteral("AssetBrowserItem"));

      icon_ = new AssetIcon(type, relative_path_, this);

      setMinimumSize(kMaxItemSize);
      setMaximumSize(kMaxItemSize);

      QVBoxLayout* layout = new QVBoxLayout();
      layout->addWidget(icon_);
      layout->setAlignment(Qt::AlignCenter);
      layout->setContentsMargins(0, 0, 0, 20);

      setLayout(layout);

      connect(
        this, 
        &AssetBrowserItem::Selected, 
        parent, 
        &AssetBrowser::OnItemSelect);

      connect(
        parent,
        &AssetBrowser::SelectionChanged,
        this,
        &AssetBrowserItem::OnSelectionChanged);
    }

    //--------------------------------------------------------------------------
    const QString& AssetBrowserItem::full_path() const
    {
      return full_path_;
    }

    //--------------------------------------------------------------------------
    compilers::AssetTypes AssetBrowserItem::type() const
    {
      return type_;
    }

    //--------------------------------------------------------------------------
    QString AssetBrowserItem::Rename(bool* was_changed, QString* old_name)
    {
      bool accepted = false;

      foundation::Path name = relative_path_.toLatin1().data();
      name = name.NoExtension();

      if (old_name != nullptr)
      {
        *old_name = name.ToString().c_str();
      }

      foundation::Path old = name;

      QString new_name = QInputDialog::getText(
        this, 
        "Rename", 
        "New name", 
        QLineEdit::Normal, 
        name.ToString().c_str(),
        &accepted);

      bool changed = 
        accepted == true && 
        name.ToString().empty() == false &&
        strcmp(new_name.toLatin1().data(), old.ToString().c_str()) != 0;

      if (was_changed != nullptr)
      {
        *was_changed = changed;
      }

      return new_name;
    }

    //--------------------------------------------------------------------------
    void AssetBrowserItem::enterEvent(QEvent* evt)
    {
      SetHighlighted(true);
    }

    //--------------------------------------------------------------------------
    void AssetBrowserItem::leaveEvent(QEvent* evt)
    {
      SetHighlighted(false);
      emit Hovered(this, false);
    }

	  //--------------------------------------------------------------------------
	  void AssetBrowserItem::mouseReleaseEvent(QMouseEvent* evt)
	  {
		  SetSelected(true);
      emit Hovered(this, true);
	  }

    //--------------------------------------------------------------------------
    void AssetBrowserItem::mouseDoubleClickEvent(QMouseEvent* evt)
    {
      if (type_ == compilers::AssetTypes::kDirectory)
      {
        emit DirectoryChanged(full_path_);
        return;
      }

      emit TryAssetImport(type_, full_path_);
    }

    //--------------------------------------------------------------------------
    void AssetBrowserItem::paintEvent(QPaintEvent* evt)
    {                                                                                                                                        
      QStyleOption style_option;                                                                                                                                                                  
      style_option.initFrom(this);

      QPainter painter(this);
      style()->drawPrimitive(QStyle::PE_Widget, &style_option, &painter, this);                                                                                                                         
    };

    //--------------------------------------------------------------------------
    void AssetBrowserItem::SetSelected(bool selected)
    {
      selected_ = selected;
      icon_->SetSelected(selected_);

      if (selected_ == true)
      {
        emit Selected(this);
      }
    }

    //--------------------------------------------------------------------------
    void AssetBrowserItem::SetHighlighted(bool highlighted)
    {
      QString highlight_col = 
        Styling::GetStyleColor(Styling::ColorRole::kHighlight);

      QString highlight_col_bg =
        Styling::GetStyleColor(Styling::ColorRole::kHighlight, 0.4f);

      QString style = highlighted == true ?
        QString(
          "#AssetBrowserItem {"
            "border: 1px solid %0;"
            "background: %1;"
            "border-radius: 10px;"
          "}")
        .arg(highlight_col, highlight_col_bg) : "";

      setStyleSheet(style);
    }

    //--------------------------------------------------------------------------
    void AssetBrowserItem::OnSelectionChanged(const AssetBrowserItem* new_item)
    {
      if (new_item != this)
      {
        SetSelected(false);
      }
    }
  }
}