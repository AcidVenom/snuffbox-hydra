#pragma once

#include <foundation/auxiliary/logger.h>

#include <QWidget>
#include <QFont>

class QPlainTextEdit;
class QTabWidget;

namespace snuffbox
{
  namespace editor
  {
    class ConsoleWidget;

    /**
    * @brief A console tab to show for each log channel, containing a
    *        simple text view
    *
    * @author Daniel Konings
    */
    class ConsoleTab : public QWidget
    {
      Q_OBJECT;

    public:
      
      /**
      * @brief Construct through the parent console widget
      *
      * @param[in] tab_index The tab index this widget lives in
      * @param[in] parent The console widget parent
      */
      ConsoleTab(int tab_index, QTabWidget* parent = nullptr);

      /**
      * @brief Writes a single log line to this tab's text field
      *
      * @param[in] text The text to append
      * @param[in] severity The severity to create the colors for
      */
      void WriteLine(
        const QString& text,
        foundation::LogSeverity severity);

    protected:

      /**
      * @brief Converts a severity to a foreground and background color
      *
      * @param[in] severity The severity to convert
      * @param[out] foreground The foreground color
      * @param[out] background The background color
      */
      static void SeverityToColors(
        foundation::LogSeverity severity, 
        QColor* foreground, 
        QColor* background);

    private:

      QTabWidget* tab_widget_; //!< The tab widget this widget lives in
      int tab_index_; //!< The tab index of this tabbed widget

      QPlainTextEdit* text_; //!< The text within the console
      int num_logs_; //!< The number of logs in this tab

      QFont spacing_font_; //!< The font used for spacing
      QFont default_font_; //!< The font used for regular block text

      static const int kFontSize_; //!< The font size of each console tab
    };
    /**
    * @brief The console widget that shows each individual log channel filtered
    *        in tabs. The console widget also serves as input for scripting
    *        and CVars
    *
    * @author Daniel Konings
    */
    class ConsoleWidget : public QWidget
    {

      Q_OBJECT;

    public:

      /**
      * @brief Construct through a parent
      *
      * @param[in] parent The parent widget
      */
      ConsoleWidget(QWidget* parent = nullptr);

      /**
      * @see Logger::RedirectOutput
      *
      * @brief Used to redirect all engine output to a particular console
      *        window
      */
      static void OnLog(
        void* ud, 
        foundation::LogChannel channel, 
        foundation::LogSeverity severity, 
        const foundation::String& msg);

    protected slots:

      /**
      * @see ConsoleWidget:::OnLog
      *
      * @remarks Does the actual logging within the right tab and with the
      *          correct colors
      */
      void OnMessageReceived(
        int channel, 
        int severity, 
        const QString& msg);

    signals:

      /**
      * @brief A signal that is emitted when a log has been received, to do
      *        thread-safe logging
      */
      void MessageReceived(int channel, int severity, const QString& msg);

    private:

      QTabWidget* tab_widget_;
      QList<ConsoleTab*> tabs_;
    };
  }
}