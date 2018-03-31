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
      * @brief Default constructor
      *
      * Sets up the internal UI to this window
      */
      MainWindow();

    private:

      Ui::MainWindow ui_; //!< The user interface definition made in Designer
    };
  }
}