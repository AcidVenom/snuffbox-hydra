#include "tools/editor/editor-widgets/console_widget.h"

#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QScrollbar>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    ConsoleTab::ConsoleTab(int tab_index, QTabWidget* parent) :
      QWidget(parent),
      tab_widget_(parent),
      tab_index_(tab_index),
      text_(nullptr),
      num_logs_(0)
    {
      QVBoxLayout* layout = new QVBoxLayout(this);
      layout->setObjectName(QStringLiteral("ConsoleTabLayout"));
      layout->setStretch(0, 1);
      
      text_ = new QPlainTextEdit(this);
      text_->setObjectName(QStringLiteral("ConsoleTabText"));
      text_->setReadOnly(true);

      QFont font;
#ifdef SNUFF_WIN32
      font.setFamily(QStringLiteral("Lucida Console"));
#elif SNUFF_LINUX
      font.setFamily(QStringLiteral("Ubuntu Mono"));
#else
      font.setFamily(QStringLiteral("Verdana"));
#endif
      
      font.setPointSize(9);

      text_->setFont(font);

      layout->addWidget(text_);
    }

    //--------------------------------------------------------------------------
    void ConsoleTab::WriteLine(
      const QString& text, 
      foundation::LogSeverity severity)
    {
      QTextCharFormat char_format;
      QTextBlockFormat block_format;

      QColor foreground, background;
      SeverityToColors(severity, &foreground, &background);

      char_format.setForeground(foreground);
      block_format.setBackground(background);

      if (severity == foundation::LogSeverity::kFatal)
      {
        char_format.setFontWeight(QFont::Weight::Bold);
      }

      QTextCursor cursor = text_->textCursor();
      QTextCharFormat old_char_format = cursor.charFormat();
      QTextBlockFormat old_block_format = cursor.blockFormat();

      cursor.movePosition(QTextCursor::End);
      cursor.setCharFormat(char_format);
      cursor.setBlockFormat(block_format);
      cursor.insertText(text + "\n");

      cursor.setBlockFormat(old_block_format);
      cursor.setCharFormat(old_char_format);

      text_->setTextCursor(cursor);

      QScrollBar* scroll_bar = text_->verticalScrollBar();
      scroll_bar->setSliderPosition(scroll_bar->maximum());

      ++num_logs_;
      tab_widget_->setTabText(tab_index_, objectName().arg(num_logs_));
    }

    //--------------------------------------------------------------------------
    void ConsoleTab::SeverityToColors(
      foundation::LogSeverity severity,
      QColor* foreground,
      QColor* background)
    {
      QColor fg(Qt::white);
      QColor bg(0, 0, 0, 0);

      switch (severity)
      {
      case foundation::LogSeverity::kDebug:
        fg = QColor(Qt::gray);
        bg = QColor(Qt::transparent);
        break;

      case foundation::LogSeverity::kInfo:
        fg = QColor(200, 200, 255);
        bg = QColor(0, 0, 80);
        break;

      case foundation::LogSeverity::kWarning:
        fg = QColor(255, 200, 0);
        bg = QColor(80, 60, 0);
        break;

      case foundation::LogSeverity::kSuccess:
        fg = QColor(80, 255, 80);
        bg = QColor(0, 80, 30);
        break;

      case foundation::LogSeverity::kError:
        fg = QColor(Qt::red);
        bg = QColor(60, 0, 0);
        break;

      case foundation::LogSeverity::kFatal:
        fg = QColor(Qt::white);
        bg = QColor(Qt::red);
        break;
      }

      if (foreground != nullptr)
      {
        *foreground = fg;
      }

      if (background != nullptr)
      {
        *background = bg;
      }
    }

    //--------------------------------------------------------------------------
    ConsoleWidget::ConsoleWidget(QWidget* parent) :
      QWidget(parent),
      tab_widget_(nullptr)
    {
      setObjectName(QStringLiteral("ConsoleWidget"));
      setMinimumSize(640, 160);

      QVBoxLayout* layout = new QVBoxLayout(this);
      layout->setStretch(0, 1);
      layout->setObjectName(QStringLiteral("ConsoleLayout"));

      setLayout(layout);

      tab_widget_ = new QTabWidget(layout->widget());
      tab_widget_->setObjectName(QStringLiteral("ConsoleTabs"));

      layout->addWidget(tab_widget_);

      QStringList names =
      {
        "All",
        "Engine",
        "Editor",
        "Player",
        "Script",
        "Builder"
      };

      Q_ASSERT(
        names.size() <= static_cast<int>(foundation::LogChannel::kNumChannels));
      
      ConsoleTab* new_tab;
      for (
        int i = 0; 
        i < static_cast<int>(foundation::LogChannel::kNumChannels);
        ++i)
      {
        new_tab = new ConsoleTab(i, tab_widget_);
        new_tab->setObjectName(names.at(i) + " (%1)");
        tab_widget_->addTab(new_tab, names.at(i) + " (0)");
        tabs_.push_back(new_tab);
      }
    }

    //--------------------------------------------------------------------------
    void ConsoleWidget::OnLog(
      void* ud,
      foundation::LogChannel channel,
      foundation::LogSeverity severity,
      const foundation::String& msg)
    {
      if (ud == nullptr)
      {
        return;
      }

      ConsoleWidget* widget = reinterpret_cast<ConsoleWidget*>(ud);
      widget->OnMessageReceived(channel, severity, QString(msg.c_str()));
    }

    //--------------------------------------------------------------------------
    void ConsoleWidget::OnMessageReceived(
      foundation::LogChannel channel,
      foundation::LogSeverity severity,
      const QString& msg)
    {
      tabs_[0]->WriteLine(msg, severity);

      if (channel != foundation::LogChannel::kUnspecified)
      {
        tabs_[static_cast<int>(channel)]->WriteLine(msg, severity);
      }
    }
  }
}