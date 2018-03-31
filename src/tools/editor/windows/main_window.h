#pragma once

#include "ui_main_window.h"
#include <qmainwindow.h>

namespace snuffbox
{
  namespace editor
  {
    /**
    * @brief The main window of the editor application
    *
    * The class is the main access point for everything related in the editor
    * and is the first element that is instantiated within the QApplication.
    * The UI itself is made in Qt5's Designer and is in the source as XML as
    * well. AUTOUIC in CMake makes sure this UI gets properly compiled every
    * time.
    *
    * @author Daniel Konings
    */
    class MainWindow : public QMainWindow
    {

    public:

      /**
      * @brief Construct through the application
      *
      * Sets up the internal UI to this window
      *
      * @param[in] app The main Qt application
      */
      MainWindow(QApplication* app);

      /**
      * @brief Applies the style I've used in "snuffbox", which are
      *        basically Steam's older colors.
      *
      * @see https://github.com/AcidVenom/snuffbox/blob/master/snuffbox/console/console.cc
      *
      * @param[in] app The main Qt application
      */
      void ApplyStyle(QApplication* app);

    private:

      Ui::MainWindow ui_; //!< The user interface definition made in Designer
      QApplication* app_; //!< A reference to the main Qt application
    };
  }
}