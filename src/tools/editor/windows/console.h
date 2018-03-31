#pragma once

#include <foundation/auxiliary/logger.h>
#include <qcolor.h>

class QTextBrowser;

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
    class Console
    {

    public:

      /**
      * @brief A log color to format the logs with based on verbosity
      *
      * @author Daniel Konings
      */
      struct LogColor
      {
        QColor foreground; //!< The foreground color
        QColor background; //!< The background color

        /**
        * @brief Converts a verbosity to a corresponding log color
        *
        * These colors are based on what I used in the oldest version of
        * snuffbox.
        *
        * @see https://github.com/AcidVenom/snuffbox/blob/master/snuffbox/console/console_widget.cc
        *
        * @param[in] verbosity The verbosity to convert
        *
        * @return The converted color
        */
        static LogColor VerbosityToColor(
          foundation::Logger::Verbosity verbosity);

        static QColor kDefaultForeground; //!< The default foreground color
        static QColor kDefaultBackground; //!< The default background color
      };

      /**
      * @brief Construct by specifying the output window this class
      *        correlates to
      *
      * This constructor will set the redirection of the Logger class in
      * foundation as well.
      *
      * @param[in] output_window A reference to the output window in the Qt
      *                          interface
      */
      Console(QTextBrowser* output_window);

      /**
      * @see Logger::OutputStream
      *
      * This function will print with different colors and filters to the
      * actual widget in the Qt interface.
      */
      static void OnReceivedMessage(
        void* ud,
        foundation::Logger::Channel channel,
        foundation::Logger::Verbosity verbosity,
        const foundation::String& message);

    protected:

      /**
      * @brief Apply the console font to the console's windows
      *
      * @param[in] output The output window to apply the font to
      */
      void ApplyFont(QTextBrowser* output);

      /**
      * @brief Writes a message to the output window
      *
      * This does the actual formatting and coloring of the output, it also
      * scrolls to the bottom of the output.
      *
      * @see Console::OnReceivedMessage
      */
      void WriteLine(
        foundation::Logger::Channel channel, 
        foundation::Logger::Verbosity verbosity,
        const char* message);

    private:

      QTextBrowser* output_window_; //!< A reference to the output window

      static QString kFontFamily_; //!< The font family used in the console
      static qreal kFontSize_; //!< The font size in points
      static qreal kTabWidth_; //!< The total width of tabs in the console
      static qreal kFatalFontWeight_; //!< The font weight for fatal logs
    };
  }
}