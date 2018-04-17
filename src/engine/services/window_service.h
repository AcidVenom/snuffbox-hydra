#pragma once

#include "engine/services/service.h"
#include "engine/application/window.h"

#include <foundation/memory/memory.h>

namespace snuffbox
{
  namespace engine
  {
    /**
    * @brief The window service, which creates the main window if the engine
    *        is ran outside of the editor
    *
    * @author Daniel Konings
    */
    class WindowService : public ServiceBase<WindowService>
    {

    public:

      /**
      * @brief Default constructor
      */
      WindowService();
      
      /**
      * @see IService::OnInitialize
      */
      foundation::ErrorCodes OnInitialize(Application& app) override;

      /**
      * @see IService::OnShutdown
      */
      void OnShutdown(Application& app) override;

      /**
      * @brief Processes the window events fired by GLFW in the underlying
      *        window
      *
      * @return Should the application exit?
      */
      bool ProcessEvents();

      /**
      * @return The window pointer
      */
      Window* GetWindow() const;

    private:

      foundation::UniquePtr<Window> window_; //!< The current window
    };
  }
}
