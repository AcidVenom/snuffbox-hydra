#include "tools/editor/editor-widgets/searcher_widget.h"

#include "tools/editor/application/editor_application.h"
#include "tools/editor/application/styling.h"

#include <QGraphicsDropShadowEffect>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QFocusEvent>
#include <QLabel>
#include <QScrollArea>
#include <QMouseEvent>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    class FocusSearchBox : public QLineEdit
    {

    public:

      //------------------------------------------------------------------------
      FocusSearchBox(QWidget* refocus, QWidget* parent) :
        QLineEdit(parent),
        refocus_(refocus)
      {

      }

    protected:

      //------------------------------------------------------------------------
      void focusOutEvent(QFocusEvent* evt) override
      {
        if (refocus_ != nullptr)
        {
          refocus_->setFocus();
        }
      }

    private:

      QWidget* refocus_;
    };

    //--------------------------------------------------------------------------
    class SearchResult : public QLabel
    {

    public:

      //------------------------------------------------------------------------
      SearchResult(
        SearcherWidget* searcher, 
        const QString& text,
        int index,
        QWidget* parent = nullptr) :
        QLabel(parent),
        searcher_(searcher),
        index_(index)
      {
        setText(text);
        setContentsMargins(4, 4, 4, 4);
      }

    protected:

      //------------------------------------------------------------------------
      void enterEvent(QEvent* evt) override
      {
        QString css = Styling::GetStyleColorCSS(Styling::ColorRole::kHighlight);
        setStyleSheet("background: " + css + "; color: black;");
      }

      //------------------------------------------------------------------------
      void leaveEvent(QEvent* evt) override
      {
        setStyleSheet("");
      }

      //------------------------------------------------------------------------
      void mouseReleaseEvent(QMouseEvent* evt) override
      {
        if (evt->button() != Qt::MouseButton::LeftButton)
        {
          return;
        }

        emit searcher_->Selected(index_);
        searcher_->CloseWindow();
      }

    private:

      SearcherWidget* searcher_;
      int index_;
    };

    //--------------------------------------------------------------------------
    const QSize SearcherWidget::kSearcherSize_ = QSize(256, 200);
    const int SearcherWidget::kMaxItems_ = 5;

    SearcherWidget* SearcherWidget::gOpenedSearcherWidget_ = nullptr;

    //--------------------------------------------------------------------------
    SearcherWidget::SearcherWidget() :
      QWidget(EditorApplication::Instance()->main_window()),
      results_(nullptr),
      results_layout_(nullptr)
    {
      setObjectName(QStringLiteral("SearcherWidget"));
      setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

      setAutoFillBackground(true);
      setBackgroundRole(QPalette::ColorRole::Background);

      QGraphicsDropShadowEffect* drop_shadow =
        new QGraphicsDropShadowEffect(this);

      drop_shadow->setBlurRadius(10);
      drop_shadow->setOffset(2, 4);
      drop_shadow->setColor(QColor(0, 0, 0, 30));

      setGraphicsEffect(drop_shadow);

      PositionWindow();

      QVBoxLayout* main_layout = new QVBoxLayout(this);
      main_layout->setObjectName(QStringLiteral("SearcherMainLayout"));
      
      QWidget* inner = new QWidget(this);
      inner->setObjectName(QStringLiteral("SearcherInnerWidget"));

      QVBoxLayout* inner_layout = new QVBoxLayout(inner);
      inner_layout->setObjectName(QStringLiteral("SearcherInnerLayout"));

      inner->setAutoFillBackground(true);
      inner->setBackgroundRole(QPalette::ColorRole::Base);
      inner->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

      FocusSearchBox* search_bar = new FocusSearchBox(this, inner);
      search_bar->setObjectName(QStringLiteral("SearcherSearchBar"));
      search_bar->setFixedHeight(24);
      search_bar->setStyleSheet("border: 1px solid green");

      connect(search_bar, &FocusSearchBox::textEdited, this, [=](const QString&)
      {
        Update(search_bar->text());
      });

      results_ = new QWidget(inner);
      results_->setObjectName(QStringLiteral("SearcherResults"));
      results_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

      results_layout_ = new QVBoxLayout(results_);
      results_layout_->setObjectName(QStringLiteral("SearcherResultsLayout"));
      results_layout_->setSpacing(0);

      QScrollArea* scroll_area = new QScrollArea(inner);
      scroll_area->setObjectName(QStringLiteral("SearcherScrollArea"));
      scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
      scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
      scroll_area->setWidgetResizable(true);
      scroll_area->setWidget(results_);
      scroll_area->setFocusPolicy(Qt::FocusPolicy::NoFocus);

      const int pad = 2;

      inner_layout->setContentsMargins(pad, pad, pad, pad);
      inner_layout->setAlignment(Qt::AlignTop);
      inner_layout->setSpacing(pad);

      inner_layout->addWidget(search_bar);
      inner_layout->addWidget(scroll_area);

      main_layout->setContentsMargins(pad, pad, pad, pad);

      main_layout->addWidget(inner);

      results_->setLayout(results_layout_);
      inner->setLayout(inner_layout);
      setLayout(main_layout);

      if (gOpenedSearcherWidget_ != nullptr)
      {
        gOpenedSearcherWidget_->CloseWindow();
      }

      gOpenedSearcherWidget_ = this;
    }

    //--------------------------------------------------------------------------
    int SearcherWidget::AddItem(const QString& search_index)
    {
      QMap<QString, int>::iterator it = search_items_.find(search_index);
      if (it != search_items_.end())
      {
        return it.value();
      }

      int index = search_items_.size();
      search_items_.insert(search_index, index);

      return index;
    }

    //--------------------------------------------------------------------------
    void SearcherWidget::Show()
    {
      Update();
      setFocus();
      show();
    }

    //--------------------------------------------------------------------------
    void SearcherWidget::CloseWindow()
    {
      if (gOpenedSearcherWidget_ == this)
      {
        close();
        deleteLater();

        gOpenedSearcherWidget_ = nullptr;
      }
    }

    //--------------------------------------------------------------------------
    void SearcherWidget::PositionWindow()
    {
      QRect geom;
      QPoint mouse_pos = QCursor::pos();

      int w = kSearcherSize_.width();
      int h = kSearcherSize_.height();

      int x1 = mouse_pos.x();
      int y1 = mouse_pos.y();
      int x2 = x1 + w;
      int y2 = y1 + h;

      MainWindow* main_window = EditorApplication::Instance()->main_window();

      const QRect& window = main_window->geometry();

      const int padding = 10;

      int dx = (window.right() - padding) - x2;
      if (dx < 0)
      {
        x1 += dx;
        x2 += dx;
      }
      
      int dy = (window.bottom() - padding) - y2;
      if (dy < 0)
      {
        y1 += dy;
        y2 += dy;
      }

      geom.setCoords(x1, y1, x2, y2);
      setGeometry(geom);
    }

    //--------------------------------------------------------------------------
    void SearcherWidget::Update(const QString& current, int max_items)
    {
      QLayoutItem* item = nullptr;
      while ((item = results_layout_->takeAt(0)) != nullptr)
      {
        delete item->widget();
      }

      auto ForEachItem = [&](
        const foundation::Function<void(const QString&, int)>& cb)
      {
        for (
          QMap<QString, int>::iterator it = search_items_.begin();
          it != search_items_.end();
          ++it)
        {
          cb(it.key(), it.value());
        }
      };

      if (current.size() == 0)
      {
        ForEachItem([&](const QString& item, int index)
        {
          SearchResult* result = new SearchResult(this, item, index, results_);
          results_layout_->addWidget(result);
        });

        return;
      }

      struct SortPair
      {
        QString item;
        int index;
        int distance;
        
        bool operator<(const SortPair& other) const
        {
          return distance < other.distance;
        }
      };

      QVector<SortPair> sorted;
      QStringList split_current = current.split(' ');

      ForEachItem([&](const QString& item, int index)
      {
        int distance = 0;

        QStringList split_item = item.split(' ');

        for (int i = 0; i < split_item.size(); ++i)
        {
          for (int j = 0; j < split_current.size(); ++j)
          {
            if (
              split_item.at(i).toLower().contains(
                split_current.at(j).toLower()))
            {
              distance -= 1000;
            }
          }
        }

        distance += FuzzyMatch(item, current);
        
        sorted.push_back(SortPair{ item, index, distance });
      });

      qSort(sorted);

      for (int j = 0; j < std::min(sorted.size(), kMaxItems_); ++j)
      {
        const SortPair& pair = sorted.at(j);
        SearchResult* result = 
          new SearchResult(this, pair.item, pair.index, results_);

        results_layout_->addWidget(result);
      }
    }

    //--------------------------------------------------------------------------
    void SearcherWidget::focusInEvent(QFocusEvent* evt)
    {
      CloseIfNotFocussed();

      QWidget::focusInEvent(evt);
    }

    //--------------------------------------------------------------------------
    void SearcherWidget::focusOutEvent(QFocusEvent* evt)
    {
      CloseIfNotFocussed();

      QWidget::focusOutEvent(evt);
    }

    //--------------------------------------------------------------------------
    void SearcherWidget::CloseIfNotFocussed()
    {
      if (geometry().contains(QCursor::pos()) == false)
      {
        CloseWindow();
      }
    }

    //--------------------------------------------------------------------------
    int SearcherWidget::FuzzyMatch(const QString& a, const QString& b)
    {
      int sa = a.size();
      int sb = b.size();

      if (sa == 0)
      {
        return sb;
      }

      if (sb == 0)
      {
        return sa;
      }

      QVector<int> costs;
      costs.resize(sb + 1);

      for (int i = 0; i <= sb; ++i)
      {
        costs[i] = i;
      }

      for (int c1 = 0; c1 < sa; ++c1)
      {
        costs[0] = c1 + 1;
        int corner = c1;

        for (int c2 = 0; c2 < sb; ++c2)
        {
          int upper = costs[c2 + 1];

          QChar aa = a.at(c1).toLower();
          QChar bb = b.at(c2).toLower();

          int cost = -1;
          if (aa == bb)
          {
            cost = corner;
          }
          else
          {
            int t = upper < corner ? upper : corner;
            int current_cost = costs[c2];

            cost = (current_cost < t ? current_cost : t) + 1;
          }

          costs[c2 + 1] = cost;
          corner = upper;
        }
      }

      return costs.at(sb);
    }
  }
}