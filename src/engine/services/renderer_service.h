#pragma once

#include "engine/services/service.h"

#include <graphics/definitions/graphics_window.h>
#include <graphics/rendering.h>

#include <foundation/memory/memory.h>

namespace snuffbox
{
  namespace engine
  {
    class CVarService;

    /**
    * @brief Used to interface the native rendering implementations and display
    *        graphics contents on the screen, in a specified window
    *
    * @author Daniel Konings
    */
    class RendererService : public ServiceBase<RendererService>
    {

    public:

      /**
      * @brief Constructs the renderer service with the window
      *        the renderer should run in
      *
      * @param[in] window The graphics window to assign
      * @param[in] cvar The CVar service
      */
      RendererService(
        const graphics::GraphicsWindow& gw,
        CVarService* cvar);

      /**
      * @brief Registers all rendering related CVars
      */
      void RegisterCVars();

      /**
      * @brief Presents the renderer to the bound graphics window
      */
      void Present();

      /**
      * @see GraphicsWindow::SizeCallback
      */
      void OnResize(uint16_t width, uint16_t height);

    protected:

      /**
      * @see IService::OnInitialize
      */
      foundation::ErrorCodes OnInitialize(Application& app) override;

      /**
      * @see IService::OnShutdown
      */
      void OnShutdown(Application& app) override;

    private:

      /**
      * @brief The underlying renderer
      */
      foundation::UniquePtr<graphics::Renderer> renderer_;

      CVarService* cvar_; //!< The current CVar service
    };
  }
}