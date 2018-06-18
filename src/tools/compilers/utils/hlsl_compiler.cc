#include "tools/compilers/utils/hlsl_compiler.h"

namespace snuffbox
{
  namespace compilers
  {
    //--------------------------------------------------------------------------
    HLSLCompiler::HLSLCompiler() :
      error_("No errors"),
      buffer_(nullptr)
    {

    }

    //--------------------------------------------------------------------------
    bool HLSLCompiler::Compile(
      const foundation::String& src, 
      AssetTypes type)
    {
      Microsoft::WRL::ComPtr<ID3DBlob> errors = nullptr;

      HRESULT result = S_OK;

      foundation::String s_type;

      switch (type)
      {
      case AssetTypes::kVertexShader:
        s_type = "vs_5_0";
        break;

      case AssetTypes::kPixelShader:
        s_type = "ps_5_0";
        break;

      case AssetTypes::kGeometryShader:
        s_type = "gs_5_0";
        break;

      default:
        s_type = "";
        break;
      }

      if (s_type.size() == 0)
      {
        return false;
      }

      result = D3DCompile(
        src.c_str(),
        src.size(),
        NULL, 
        NULL, 
        NULL,
        "main",
        s_type.c_str(),
        D3DCOMPILE_ENABLE_STRICTNESS,
        0,
        buffer_.GetAddressOf(),
        errors.GetAddressOf()
      );

      if (errors != nullptr)
      {
        error_ = reinterpret_cast<const char*>(errors->GetBufferPointer());

        return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------
    const foundation::String& HLSLCompiler::error() const
    {
      return error_;
    }

    //--------------------------------------------------------------------------
    const uint8_t* HLSLCompiler::GetBuffer(size_t* len) const
    {
      if (buffer_ == nullptr)
      {
        if (len != nullptr)
        {
          *len = 0;
        }

        return nullptr;
      }

      if (len != nullptr)
      {
        *len = buffer_->GetBufferSize();
      }

      return reinterpret_cast<uint8_t*>(buffer_->GetBufferPointer());
    }
  }
}