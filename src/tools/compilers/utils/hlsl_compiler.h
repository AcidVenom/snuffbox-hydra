#pragma once

#include "tools/compilers/definitions/asset_types.h"

#include <wrl/client.h>
#include <d3dcompiler.h>

namespace snuffbox
{
  namespace compilers
  {
    /**
    * @brief Used to compile HLSL shaders into their respective binary format
    *
    * @author Daniel Konings
    */
    class HLSLCompiler
    {
      
    public:

      HLSLCompiler();

      /**
      * @brief Compiles HLSL shaders using D3DCompile
      *
      * @param[in] src The source HLSL code as a string
      * @param[in] type The type of the shader, valid arguments are:
      *                 - AssetTypes::kVertexShader
      *                 - AssetTypes::kPixelShader
      *                 - AssetTypes::kGeometryShader
      *
      * @return Was the compilation a success?
      */
      bool Compile(
        const foundation::String& src, 
        AssetTypes type);

      /**
      * @return The current error message
      */
      const foundation::String& error() const;

      /**
      * @return The buffer of the compiler
      *
      * @param[out] len The length of the buffer
      */
      const uint8_t* GetBuffer(size_t* len) const;

    private:

      foundation::String error_; //!< The current error message
      Microsoft::WRL::ComPtr<ID3DBlob> buffer_; //!< The buffer of the compiler
    };
  }
}