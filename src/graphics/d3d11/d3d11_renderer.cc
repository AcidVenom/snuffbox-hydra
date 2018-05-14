#include "graphics/d3d11/d3d11_renderer.h"
#include "graphics/d3d11/d3d11_utils.h"

#include <foundation/auxiliary/logger.h>

#include <d3d11.h>
#include <dxgi.h>
#include <comdef.h>

namespace snuffbox
{
  namespace graphics
  {
    //--------------------------------------------------------------------------
    D3D11Renderer::D3D11Renderer(const GraphicsWindow& window) :
      IRenderer(window)
    {

    }

    //--------------------------------------------------------------------------
    bool D3D11Renderer::Initialize()
    {
      adapter_ = FindAdapter();

      if (CreateDevice(adapter_.Get()) == false)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kEngine,
          foundation::LogSeverity::kFatal,
          "Could not create the D3D11 rendering device"
          );

        return false;
      }

      if (CreateBackBuffer(device_.Get()) == false)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kEngine,
          foundation::LogSeverity::kFatal,
          "Could not create the D3D11 back buffer"
          );

        return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------
    void D3D11Renderer::Clear(const glm::vec4& color)
    {
      context_->ClearRenderTargetView(back_buffer_.Get(), &color[0]);
    }

    //--------------------------------------------------------------------------
    void D3D11Renderer::Clear(const RenderTarget& rt, const glm::vec4& color)
    {
      //!< @todo Implement this function once render targets are a thing
    }

    //--------------------------------------------------------------------------
    void D3D11Renderer::SetViewport(const Viewport& vp)
    {
      D3D11_VIEWPORT viewport = D3D11Utils::EmptyStruct<D3D11_VIEWPORT>();

      viewport.TopLeftX = vp.x;
      viewport.TopLeftY = vp.y;
      viewport.Width = vp.width;
      viewport.Height = vp.height;

      context_->RSSetViewports(1, &viewport);
    }

    //--------------------------------------------------------------------------
    bool D3D11Renderer::CreateBackBuffer(
      ID3D11Device* device)
    {
      ComPtr<ID3D11Texture2D> b;
      
      if (D3D11Utils::Check(swap_chain_->GetBuffer(
        0,
        __uuidof(ID3D11Texture2D),
        reinterpret_cast<void**>(b.GetAddressOf()))) == false)
      {
        return false;
      }

      if (D3D11Utils::Check(device->CreateRenderTargetView(
        b.Get(), 
        nullptr, 
        back_buffer_.GetAddressOf())) == false)
      {
        return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------
    void D3D11Renderer::Present(bool vsync)
    {
      context_->OMSetRenderTargets(1, back_buffer_.GetAddressOf(), nullptr);
      swap_chain_->Present(vsync == true ? 1 : 0, 0);
      context_->OMSetRenderTargets(0, nullptr, nullptr);
    }

    //--------------------------------------------------------------------------
    ComPtr<IDXGIAdapter> D3D11Renderer::FindAdapter()
    {
      ComPtr<IDXGIFactory> factory;
      ComPtr<IDXGIAdapter> adapter;

      if (D3D11Utils::Check(CreateDXGIFactory(
        __uuidof(IDXGIFactory), 
        reinterpret_cast<void**>(factory.GetAddressOf()))) == false)
      {
        return nullptr;
      }

      if (D3D11Utils::Check(factory->EnumAdapters(
        0, 
        adapter.GetAddressOf())) == false)
      {
        return nullptr;
      }

      DXGI_ADAPTER_DESC desc = D3D11Utils::EmptyStruct<DXGI_ADAPTER_DESC>();
      if (D3D11Utils::Check(adapter->GetDesc(&desc)) == false)
      {
        return nullptr;
      }

      size_t vram = desc.DedicatedVideoMemory / 1024 / 1024;
      size_t shared_memory = desc.SharedSystemMemory / 1024 / 1024;
      WCHAR* adapter_name = desc.Description;

      _bstr_t name = adapter_name;

      foundation::Logger::LogVerbosity<2>(
        foundation::LogChannel::kEngine,
        foundation::LogSeverity::kInfo,
        "Default display adapter:\n\n\
        \tVideo card name: {0}\n\
        \tVirtual memory: {1}\n\
        \tShared system memory: {2}\n",
        name,
        vram,
        shared_memory
        );

      return adapter;
    }

    //--------------------------------------------------------------------------
    bool D3D11Renderer::CreateDevice(IDXGIAdapter* adapter)
    {
      const GraphicsWindow& gw = window();

      DXGI_SWAP_CHAIN_DESC desc = 
        D3D11Utils::EmptyStruct<DXGI_SWAP_CHAIN_DESC>();

      desc.BufferCount = 1;
      desc.BufferDesc.Width = static_cast<uint32_t>(gw.width);
      desc.BufferDesc.Height = static_cast<uint32_t>(gw.height);
      desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
      desc.BufferDesc.RefreshRate.Numerator = 60;
      desc.BufferDesc.RefreshRate.Denominator = 1;
      desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      desc.OutputWindow = reinterpret_cast<HWND>(gw.handle);
      desc.SampleDesc.Count = 1;
      desc.SampleDesc.Quality = 0;
      desc.Windowed = TRUE;
      desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
      desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
      desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
      desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

      D3D_FEATURE_LEVEL feature_levels_requested[] =
      {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_0
      };

      D3D_FEATURE_LEVEL feature_levels_supported;

      UINT device_flags = 0;

#ifdef _DEBUG
      device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

      if (D3D11Utils::Check(D3D11CreateDeviceAndSwapChain(
        adapter,
        D3D_DRIVER_TYPE_UNKNOWN,
        nullptr,
        device_flags,
        feature_levels_requested,
        1,
        D3D11_SDK_VERSION,
        &desc,
        swap_chain_.GetAddressOf(),
        device_.GetAddressOf(),
        &feature_levels_supported,
        context_.GetAddressOf()
      )) == false)
      {
        return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------
    void D3D11Renderer::OnResizeImpl(uint16_t width, uint16_t height)
    {
      back_buffer_.Reset();

      if (D3D11Utils::Check(swap_chain_->ResizeBuffers(
        1,
        0,
        0,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH)) == false)
      {
        foundation::Logger::LogVerbosity<2>(
          foundation::LogChannel::kEngine,
          foundation::LogSeverity::kError,
          "Could not resize renderer buffers"
          );

        return;
      }

      if (CreateBackBuffer(device_.Get()) == false)
      {
        foundation::Logger::LogVerbosity<2>(
          foundation::LogChannel::kEngine,
          foundation::LogSeverity::kError,
          "Could not recreate back buffer after resizing renderer buffers"
          );
      }
    }
  }
}