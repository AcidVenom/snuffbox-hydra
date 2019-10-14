#include "tools/editor/asset-browser/asset_browser_item.h"

#include "tools/editor/asset-browser/asset_icon.h"
#include "tools/editor/asset-browser/asset_browser.h"
#include "tools/editor/application/styling.h"

#include <QVBoxLayout>
#include <QStyle>
#include <QPainter>
#include <QStyleOption>

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
      AssetBrowser* parent)
      :
      QWidget(parent),
      type_(type),
      path_(path),
      icon_(nullptr),
      selected_(false)
    {
      setObjectName(QStringLiteral("AssetBrowserItem"));

      icon_ = new AssetIcon(type, path, this);

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
    void AssetBrowserItem::enterEvent(QEvent* evt)
    {
      SetHighlighted(true);
    }

    //--------------------------------------------------------------------------
    void AssetBrowserItem::leaveEvent(QEvent* evt)
    {
      SetHighlighted(false);
    }

	  //--------------------------------------------------------------------------
	  void AssetBrowserItem::mouseReleaseEvent(QMouseEvent* evt)
	  {
		  SetSelected(true);
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