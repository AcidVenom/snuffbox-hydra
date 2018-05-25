#pragma once

#include <foundation/definitions/logging.h>
#include <foundation/containers/string.h>

#include <qobject.h>
#include <qcolor.h>

class QTextBrowser;
class QTabWidget;

namespace snuffbox
{
  namespace editor
  {
    /**
    * @brief The console window contained within the MainWindow
    *
    * Used to redirect all foundation logs to this window and execute commands
    * and scripting code within the engine.
    *
    * @author Daniel Konings
    */
    class Console : public QObject
    {

      Q_OBJECT

    public:

      /**
      * @brief Construct by specifying the output window this class
      *        correlates to
      *
      * This constructor will set the redirection of the Logger class in
      * foundation as well. The output_windows parameter size should match
      * the number of logging channels that exist, @see Logger::Channels
      *
      * @param[in] tab The tabs the output windows reside in
      * @param[in] output_windows A reference to the output windows in the Qt
      *                           interface
      */
      Console(QTabWidget* tab, QTextBrowser** output_windows);

      /**
      * @see Logger::OutputStream
      *
      * This function will print with different colors and filters to the
      * actual widget in the Qt interface.
      */
      static void OnReceivedMessage(
        void* ud,
        foundation::LogChannel channel,
        foundation::LogSeverity severity,
        const foundation::String& message);

    protected:

      /**
      * @brief Sets up all output windows for use
      *
      * @param[in] tab The tabs the output windows reside in
      * @param[in] outputs The output windows passed from the MainWindow
      */
      void SetupOutputWindows(QTabWidget* tab, QTextBrowser** outputs);

      /**
      * @brief Sets the log count for a specified channel
      *
      * @param[in] channel The channel to set the log count of
      * @param[in] count The count to set
      */
      void SetLogCount(foundation::LogChannel channel, int count);

      /**
      * @brief Apply the console font to the console's windows
      *
      * @param[in] output The output window to apply the font to
      */
      void ApplyFont(QTextBrowser* output);

    signals:

      /**
      * @see Console::OnReceivedMessage
      */
      void OnLog(
        int channel,
        int severity,
        QString message);

    private slots:

      /**
      * @brief Writes a message to the output window
      *
      * This does the actual formatting and coloring of the output, it also
      * scrolls to the bottom of the output.
      *
      * @see Console::OnReceivedMessage
      */
      void WriteLine(
        int channel, 
        int severity,
        QString message);

    private:

      /**
      * @brief Used to store information about an output window
      *
      * The original name is used to add a log count to the name in the tab
      *
      * @author Daniel Konings
      */
      struct OutputWindow
      {
        QTextBrowser* window; //!< The window pointer
        QTabWidget* tab; //!< The tab widget this output window is a child of
        int tab_index; //!< The tab index this output window exists in
        QString original_name; //!< The original name of the window
        int log_count; //!< The count for the number of logs in this window
      };

      /**
      * @brief A reference to the output window for each respective channel
      */
      OutputWindow output_windows_[
        static_cast<int>(foundation::LogChannel::kNumChannels) + 1];

      static QString kFontFamily_; //!< The font family used in the console
      static qreal kFontSize_; //!< The font size in points
      static qreal kTabWidth_; //!< The total width of tabs in the console
      static qreal kFatalFontWeight_; //!< The font weight for fatal logs
    };
  }
}