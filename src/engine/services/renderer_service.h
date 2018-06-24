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
    class MeshComponent;
    class TransformComponent;
    class MeshRendererComponent;
    class CameraComponent;

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
      */
      RendererService(const graphics::GraphicsWindow& gw);

      /**
      * @brief Presents the renderer and its data to the bound graphics window
      *
      * @param[in] dt The current delta time of the application
      */
      void Render(float dt);

      /**
      * @return The renderer loader of the current rendering API
      */
      graphics::IRendererLoader* GetLoader();

      /**
      * @see GraphicsWindow::SizeCallback
      */
      void OnResize(uint16_t width, uint16_t height);

      /**
      * @brief Queues a camera for rendering
      *
      * @param[in] transform The transformation of the camera
      * @param[in] camera The camera to render with
      */
      void Queue(
        TransformComponent* transform,
        CameraComponent* camera);

      /**
      * @brief Queues a mesh renderer component for rendering of an entity's
      *        MeshComponent
      *
      * @param[in] mesh The mesh to render
      * @param[in] transform The transformation to apply
      * @param[in] renderer The mesh renderer to render the mesh with
      */
      void Queue(
        MeshComponent* mesh, 
        TransformComponent* transform, 
        MeshRendererComponent* renderer);

    protected:

      /**
      * @see IService::OnInitialize
      */
      foundation::ErrorCodes OnInitialize(Application& app) override;

      /**
      * @see IService::OnShutdown
      */
      void OnShutdown(Application& app) override;

      /**
      * @see IService::RegisterCVars
      */
      void RegisterCVars(CVarService* cvar) override;

    private:

      /**
      * @brief The underlying renderer
      */
      foundation::UniquePtr<graphics::Renderer> renderer_;

      CVarService* cvar_; //!< The current CVar service

      const static bool kDefaultVsync_; //!< The default r_vsync value
      const static double kDefaultWidth_; //!< The default r_width value
      const static double kDefaultHeight_; //!< The default r_height value
    };
  }
}