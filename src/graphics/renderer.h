#pragma once

#include "graphics/definitions/graphics_window.h"

namespace snuffbox
{
  namespace graphics
  {
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
      * @brief Presents the renderer in the window after rendering
      */
      virtual void Present() = 0;

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

    protected:

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
    };
  }
}