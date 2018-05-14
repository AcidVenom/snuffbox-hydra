#pragma once

#include "graphics/renderer.h"
#include "graphics/definitions/com_ptr.h"

#include <foundation/containers/string.h>

struct IDXGIAdapter;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;

namespace snuffbox
{
  namespace graphics
  {
    /**
    * @brief The DirectX 11 renderer for use on Windows platforms
    *
    * @author Daniel Konings
    */
    class D3D11Renderer : public IRenderer
    {
      
    public:

      /**
      * @see IRenderer::IRenderer
      */
      D3D11Renderer(const GraphicsWindow& window);

      /**
      * @see IRenderer::Initialize
      */
      bool Initialize() override;

      /**
      * @see IRenderer::Clear
      */
      void Clear(const glm::vec4& color) override;

      /**
      * @see IRenderer::Clear
      */
      void Clear(const RenderTarget& rt, const glm::vec4& color) override;

      /**
      * @see IRenderer::SetViewport
      */
      void SetViewport(const Viewport& vp) override;

      /**
      * @see IRenderer::Present
      */
      void Present(bool vsync) override;

    protected:

      /**
      * @brief Attempts to find the best adapter based on VRAM scoring
      *
      * @remarks If no appropriate adapters are found, this function will
      *          return nullptr
      *
      * @return The found adapter
      */
      static ComPtr<IDXGIAdapter> FindAdapter();

      /**
      * @brief This method will create the actual ID3D11Device and the
      *        corresponding swap chain
      *
      * @param[in] adapter The adapter to create the device and swap chain
      *                    with
      *
      * @return Was the creation a success?
      */
      bool CreateDevice(IDXGIAdapter* adapter);

      /**
      * @brief Creates the back buffer object to render into
      *
      * @param[in] device The device to create the buffer with
      *
      * @return Was the creation a success?
      */
      bool CreateBackBuffer(ID3D11Device* device);

      /**
      * @see IRenderer::OnResizeImpl
      */
      void OnResizeImpl(uint16_t width, uint16_t height) override;

    private:

      ComPtr<IDXGIAdapter> adapter_; //!< The adapter to use with this renderer
      ComPtr<ID3D11Device> device_; //!< The device of this renderer
      ComPtr<IDXGISwapChain> swap_chain_; //!< The swap chain of this renderer
      ComPtr<ID3D11DeviceContext> context_; //!< The context of this renderer

      ComPtr<ID3D11RenderTargetView> back_buffer_; //!< The back buffer
    };
  }
}