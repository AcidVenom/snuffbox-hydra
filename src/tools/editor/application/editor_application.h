#pragma once

#include <engine/application/application.h>

#include <QApplication>
#include <QSettings>

namespace snuffbox
{
  namespace editor
  {
    /**
    * @brief The application instance for the editor version of the engine
    *
    * @author Daniel Konings
    */
    class EditorApplication : public engine::Application
    {

    public:

      /**
      * @see Application::Application
      *
      * @param[in] qapp The Qt application to update each frame
      */
      EditorApplication(int argc, char** argv, const Configuration& cfg);

      /**
      * @see Application::Instance
      */
      static EditorApplication* Instance();

      /**
      * @see Application::Run
      *
      * Makes sure the engine is ran from within the Qt interfaces, instead
      * of creating a separate window for the renderer instance
      */
      foundation::ErrorCodes Run() override;

      /**
      * @return The global settings of the application
      */
      QSettings& GlobalSettings() const;

    protected:

      /**
      * @see Application::CreateRenderer
      */
      void CreateRenderer() override;

    private:

      QApplication qapp_; //!< The Qt application
    };
  }
}