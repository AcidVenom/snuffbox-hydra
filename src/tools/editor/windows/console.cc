#include "tools/editor/windows/console.h"
#include "tools/editor/definitions/editor_colors.h"

#include <foundation/auxiliary/logger.h>

#include <qtextbrowser.h>
#include <qscrollbar.h>
#include <qtabwidget.h>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    QString Console::kFontFamily_ = "Consolas";
    qreal Console::kFontSize_ = 10;
    qreal Console::kTabWidth_ = 2;
    qreal Console::kFatalFontWeight_ = 99;

    //--------------------------------------------------------------------------
    Console::Console(QTabWidget* tab, QTextBrowser** output_windows)
    {
      foundation::Logger::RedirectOutput(OnReceivedMessage, this);

      SetupOutputWindows(tab, output_windows);
      connect(this, &Console::OnLog, this, &Console::WriteLine);

      foundation::Logger::Log(
        foundation::LogChannel::kEditor,
        foundation::LogSeverity::kInfo,
        "Welcome!");
    }

    //--------------------------------------------------------------------------
    void Console::OnReceivedMessage(
      void* ud,
      foundation::LogChannel channel,
      foundation::LogSeverity verbosity,
      const foundation::String& message
      )
    {
      Console* console = reinterpret_cast<Console*>(ud);

      console->OnLog(
        static_cast<int>(channel), 
        static_cast<int>(verbosity), 
        message.c_str());
    }

    //--------------------------------------------------------------------------
    void Console::SetupOutputWindows(QTabWidget* tab, QTextBrowser** outputs)
    {
      QTextBrowser* output_window;

      for (
        int i = 0;
        i < static_cast<int>(foundation::LogChannel::kNumChannels);
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

        SetLogCount(static_cast<foundation::LogChannel>(i), 0);
      }
    }

    //--------------------------------------------------------------------------
    void Console::SetLogCount(foundation::LogChannel channel, int count)
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
      int channel,
      int severity,
      QString message)
    {
      foundation::LogChannel ch 
        = static_cast<foundation::LogChannel>(channel);
      foundation::LogSeverity sev 
        = static_cast<foundation::LogSeverity>(severity);

      OutputWindow& window = output_windows_[channel];
      QTextBrowser* output_window = window.window;

      SetLogCount(ch, ++window.log_count);

      ColorFormat color = EditorColors::ConsoleColor(sev);

      QTextCursor cursor = output_window->textCursor();

      QTextBlockFormat format;
      format.setBackground(color.background);

      QTextCharFormat text_format;
      text_format.setForeground(color.foreground);

      if (sev == foundation::LogSeverity::kFatal)
      {
        text_format.setFontWeight(kFatalFontWeight_);
      }

      cursor.movePosition(QTextCursor::End);
      cursor.setBlockFormat(format);
      cursor.setCharFormat(text_format);
      cursor.insertText(message + '\n');
      format.setBackground(QColor(0, 0, 0, 0));
      cursor.setBlockFormat(format);

      output_window->setTextCursor(cursor);

      QScrollBar* scroll_bar = output_window->verticalScrollBar();
      scroll_bar->setSliderPosition(scroll_bar->maximum());

      if (ch != foundation::LogChannel::kUnspecified)
      {
        OnLog(
          static_cast<int>(foundation::LogChannel::kUnspecified),
          severity,
          message);
      }
    }
  }
}