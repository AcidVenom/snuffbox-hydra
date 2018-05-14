#pragma once

namespace snuffbox
{
  namespace graphics
  {
    /**
    * @brief The different texture formats that are supported
    *
    * Texture formats are both used for regular textures, both 2D and 3D and
    * render targets.
    */
    enum class TextureFormats
    {
      kRGBA8Unorm,
      kRGB8Unorm,
      kRG8Unorm,
      kR8Unorm,
      kRGBA16F,
      kRGB16F,
      kRG16F,
      kR16F,
      kRGBA32F,
      kRGB32F,
      kRG32F,
      kR32F,
      kRGBA16Uint,
      kRGB16Uint,
      kRG16Uint,
      kR16Uint,
      kRGBA32Uint,
      kRGB32Uint,
      kRG32Uint,
      kR32Uint,
      kRGBA16Int,
      kRGB16Int,
      kRG16Int,
      kR16Int,
      kRGBA32Int,
      kRGB32Int,
      kRG32Int,
      kR32Int,
    };
  }
}