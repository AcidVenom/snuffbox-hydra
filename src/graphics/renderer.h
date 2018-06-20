#pragma once

#include "graphics/definitions/graphics_window.h"
#include "graphics/definitions/viewport.h"
#include "graphics/definitions/render_target.h"
#include "graphics/definitions/camera.h"
#include "graphics/definitions/frame_data.h"
#include "graphics/definitions/draw_command.h"

#include <foundation/containers/queue.h>
#include <foundation/containers/vector.h>

#include <glm/glm.hpp>

namespace snuffbox
{
  namespace graphics
  {
    class IRendererLoader;

    /**
    * @brief The renderer interface for every native rendering API to use
    *
    * @author Daniel Konings
    */
    class IRenderer
    {

    public:

      /**
      * @brief Construct a ready-for-use renderer on a specified window
      *
      * @param[in] window The window information to initially construct
      *                   the renderer with
      */
      IRenderer(const GraphicsWindow& window);

      /**
      * @brief Initializes the renderer for use
      *
      * @return Was the initialization of the renderer a success?
      */
      virtual bool Initialize() = 0;

      /**
      * @brief Sets the viewport of the renderer to use for the upcoming draw
      *        calls
      *
      * @param[in] vp The viewport to set
      */
      virtual void SetViewport(const Viewport& vp) = 0;

      /**
      * @brief Clears the back buffer
      *
      * @param[in] color The color to clear the back buffer with
      */
      virtual void Clear(const glm::vec4& color) = 0;

      /**
      * @brief Clears a specified render target
      *
      * @param[in] rt The render target to clear
      * @param[in] color The color to clear the render target with
      */
      virtual void Clear(const RenderTarget& rt, const glm::vec4& color) = 0;

      /**
      * @brief Queue a camera for rendering
      *
      * @param[in] camera The camera to render the scene with
      */
      void Queue(const Camera& camera);

      /**
      * @brief Queue a draw command to render individual meshes
      *
      * This buffer is dynamically sized. Whenever a command gets queued,
      * the number of elements increases. Thus, the renderer knows which
      * commands are valid and which aren't.
      *
      * This is used as the mesh command queues are used per camera, so we
      * cannot use a regular queue with popping.
      *
      * @remarks The pre-allocated size of the buffer might change,
      *          because when it overflows it is dynamically resized to 
      *          twice its size.
      *
      * @param[in] cmd The command
      */
      void Queue(const DrawCommand& cmd);

      /**
      * @brief Updates the time of the renderer
      *
      * @param[in] dt The current delta time of the application
      */
      void Tick(float dt);

      /**
      * @brief Render each draw command per camera
      *
      * @remarks This call "flushes" all render queues (cameras and commands)
      */
      void Render();

      /**
      * @brief Presents the renderer in the window after rendering
      *
      * @param[in] vsync Should vertical sync be enabled?
      */
      virtual void Present(bool vsync) = 0;

      /**
      * @brief This method should be called when the window is resized to
      *        resize the underlying buffers of the renderer as well
      *
      * @remarks This updates the underlying IRenderer::window_ struct,
      *          after which IRenderer::OnResizeImpl is called for the derived
      *          class
      *
      * @param[in] width The new width of the window
      * @param[in] height The new height of the window
      */
      void OnResize(uint16_t width, uint16_t height);

      /**
      * @brief Retrieves the renderer loader of this renderer
      *
      * @remarks Used to create GPU resources from outside of the API
      *
      * @return The renderer loader
      */
      virtual IRendererLoader* GetLoader() = 0;

    protected:

      /**
      * @brief Called when a frame is started during IRenderer::Render
      */
      virtual void OnStartFrame() = 0;

      /**
      * @brief Draws the current queue with a specified camera
      *
      * @param[in] camera The camera data to render with
      */
      void Draw(const Camera& camera);

      /**
      * @brief Draws the current draw command onto the current camera's render
      *        targets
      *
      * @param[in] cmd The command to draw
      * @param[in] camera The current camera
      */
      void Draw(const DrawCommand& cmd, const Camera& camera);

      /**
      * @brief Sets the per frame data into a constant buffer internally
      *
      * @param[in] The frame data to set
      *
      * @see IRenderer::Draw
      */
      virtual void SetFrameData(const PerFrameData& pfd) = 0;

      /**
      * @brief Sets the per object data into a constant buffer internally
      *
      * @param[in] The object data to set
      *
      * @see IRenderer::Draw
      */
      virtual void SetFrameData(const PerObjectData& pod) = 0;

      /**
      * @see IRenderer::OnResize
      */
      virtual void OnResizeImpl(uint16_t width, uint16_t height) = 0;

      /**
      * @return The window this renderer belongs to
      */
      const GraphicsWindow& window() const;

    public:

      /**
      * @brief Virtual destructor
      */
      virtual ~IRenderer();

    private:

      GraphicsWindow window_; //!< The window this renderer belongs to

      foundation::Queue<Camera> cameras_; //!< The cameras to render with
      foundation::Vector<DrawCommand> commands_; //!< The commands to render

      size_t num_commands_; //!< The number of draw commands
      float time_; //!< The current elapsed time of the application

      static const size_t kDefaultQueueSize_; //!< The default queue size
    };
  }
}