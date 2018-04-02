#include "tools/editor/windows/console.h"

#include <qtextbrowser.h>
#include <qscrollbar.h>
#include <qtabwidget.h>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    QColor Console::LogColor::kDefaultForeground = QColor(0, 0, 0, 255);
    QColor Console::LogColor::kDefaultBackground = QColor(0, 0, 0, 0);

    //--------------------------------------------------------------------------
    QString Console::kFontFamily_ = "Consolas";
    qreal Console::kFontSize_ = 10;
    qreal Console::kTabWidth_ = 2;
    qreal Console::kFatalFontWeight_ = 99;

    //--------------------------------------------------------------------------
    Console::LogColor Console::LogColor::VerbosityToColor(
      foundation::Logger::Verbosity verbosity)
    {
      LogColor color;
      color.foreground = kDefaultForeground;
      color.background = kDefaultBackground;

      switch (verbosity)
      {

      case foundation::Logger::Verbosity::kDebug:
        color.foreground = QColor(128, 128, 128);
        color.background = QColor(0, 0, 0, 0);
        break;

      case foundation::Logger::Verbosity::kInfo:
        color.foreground = QColor(255, 255, 255);
        color.background = QColor(30, 30, 30, 30);
        break;

      case foundation::Logger::Verbosity::kWarning:
        color.foreground = QColor(255, 185, 0);
        color.background = QColor(70, 65, 15);
        break;

      case foundation::Logger::Verbosity::kSuccess:
        color.foreground = QColor(35, 255, 0);
        color.background = QColor(70, 120, 55);
        break;

      case foundation::Logger::Verbosity::kError:
        color.foreground = QColor(255, 0, 0);
        color.background = QColor(60, 0, 0);
        break;

      case foundation::Logger::Verbosity::kFatal:
        color.foreground = QColor(255, 255, 255);
        color.background = QColor(255, 0, 0);
        break;

      }

      return color;
    }

    //--------------------------------------------------------------------------
    Console::Console(QTabWidget* tab, QTextBrowser** output_windows)
    {
      foundation::Logger::RedirectOutput(OnReceivedMessage, this);

      SetupOutputWindows(tab, output_windows);

      foundation::Logger::Log(
        foundation::Logger::Channel::kEditor,
        foundation::Logger::Verbosity::kInfo,
        "Welcome!");
    }

    //--------------------------------------------------------------------------
    void Console::OnReceivedMessage(
      void* ud,
      foundation::Logger::Channel channel,
      foundation::Logger::Verbosity verbosity,
      const foundation::String& message
      )
    {
      Console* console = reinterpret_cast<Console*>(ud);

      console->WriteLine(channel, verbosity, message.c_str());
    }

    //--------------------------------------------------------------------------
    void Console::SetupOutputWindows(QTabWidget* tab, QTextBrowser** outputs)
    {
      QTextBrowser* output_window;

      for (
        int i = 0;
        i < static_cast<int>(foundation::Logger::Channel::kNumChannels);
        ++i)
      {
        output_window = outputs[i];
        ApplyFont(output_window);

        output_windows_[i] = OutputWindow 
        {
          output_window,
          tab,
          i,
          tab->tabText(i),
          0
        };

        SetLogCount(static_cast<foundation::Logger::Channel>(i), 0);
      }
    }

    //--------------------------------------------------------------------------
    void Console::SetLogCount(foundation::Logger::Channel channel, int count)
    {
      OutputWindow& window = output_windows_[static_cast<int>(channel)];
      window.log_count = count;
      window.tab->setTabText(window.tab_index, 
        window.original_name + " (" + std::to_string(count).c_str() + ")");
    }

    //--------------------------------------------------------------------------
    void Console::ApplyFont(QTextBrowser* output)
    {
      QFont monospace;
      monospace.setFamily(kFontFamily_);
      monospace.setStyleHint(QFont::Monospace);
      monospace.setFixedPitch(true);
      monospace.setPointSize(kFontSize_);

      output->setFont(monospace);

      QFontMetrics metrics(monospace);
      output->setTabStopWidth(kTabWidth_ * metrics.width(" "));
    }

    //--------------------------------------------------------------------------
    void Console::WriteLine(
      foundation::Logger::Channel channel,
      foundation::Logger::Verbosity verbosity,
      const char* message)
    {
      OutputWindow& window = output_windows_[static_cast<int>(channel)];
      QTextBrowser* output_window = window.window;

      SetLogCount(channel, ++window.log_count);

      LogColor color = LogColor::VerbosityToColor(verbosity);

      QTextCursor cursor = output_window->textCursor();

      QTextBlockFormat format;
      format.setBackground(color.background);

      QTextCharFormat text_format;
      text_format.setForeground(color.foreground);

      if (verbosity == foundation::Logger::Verbosity::kFatal)
      {
        text_format.setFontWeight(kFatalFontWeight_);
      }

      cursor.movePosition(QTextCursor::End);
      cursor.setBlockFormat(format);
      cursor.setCharFormat(text_format);
      cursor.insertText(QString(message) + '\n');
      format.setBackground(QColor(0, 0, 0, 0));
      cursor.setBlockFormat(format);

      output_window->setTextCursor(cursor);

      QScrollBar* scroll_bar = output_window->verticalScrollBar();
      scroll_bar->setSliderPosition(scroll_bar->maximum());

      if (channel != foundation::Logger::Channel::kUnspecified)
      {
        WriteLine(
          foundation::Logger::Channel::kUnspecified,
          verbosity,
          message);
      }
    }
  }
}