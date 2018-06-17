#include "tools/compilers/utils/spirv_cross.h"

#include <spirv_hlsl.hpp>
#include <spirv_glsl.hpp>

#include <vector>

#ifdef SNUFF_WIN32
#include <d3dcompiler.h>
#include <wrl/client.h>
#endif

namespace snuffbox
{
  namespace compilers
  {
#ifdef SNUFF_WIN32
    //--------------------------------------------------------------------------
    ID3DBlob* CompileHLSLSource(
      const foundation::String& src,
      AssetTypes type,
      foundation::String* err)
    {
      Microsoft::WRL::ComPtr<ID3DBlob> errors = nullptr;
      ID3DBlob* buffer = nullptr;

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
        if (err != nullptr)
        {
          *err = "Invalid shader type";
        }

        return nullptr;
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
        &buffer,
        errors.GetAddressOf()
      );

      if (errors != nullptr)
      {
        if (err != nullptr)
        {
          *err = reinterpret_cast<const char*>(errors->GetBufferPointer());
        }

        return nullptr;
      }

      return buffer;
    }
#endif

    //--------------------------------------------------------------------------
    const int SPIRVCross::kGLSLVersion_ = 330;
    const int SPIRVCross::kShaderModel_ = 50;

    //--------------------------------------------------------------------------
    bool SPIRVCross::Compile(
      const uint8_t* spirv, 
      size_t len,
      AssetTypes type,
      ShaderData* data)
    {
      SourceData sources;
      if (ToSourceCode(spirv, len, &sources) == false)
      {
        error_ = "Could not convert SPIRV into source code";
        return false;
      }

      if (data != nullptr)
      {
#ifdef SNUFF_WIN32
        data->has_hlsl = true;

        ID3DBlob* blob;
        if ((blob = CompileHLSLSource(sources.hlsl, type, &error_)) == nullptr)
        {
          return false;
        }

        size_t size = blob->GetBufferSize();
        data->hlsl.resize(size);

        for (size_t i = 0; i < size; ++i)
        {
          data->hlsl.at(i) = 
            *(reinterpret_cast<const uint8_t*>(blob->GetBufferPointer()) + i);
        }

        blob->Release();
#else
        data->has_hlsl = false;
#endif
        data->glsl = sources.glsl;
      }

      return true;
    }

    //--------------------------------------------------------------------------
    const foundation::String& SPIRVCross::error() const
    {
      return error_;
    }

    //--------------------------------------------------------------------------
    bool SPIRVCross::ToSourceCode(
      const uint8_t* spirv,
      size_t len,
      SourceData* data)
    {
      size_t size = len / 4;
      std::vector<uint32_t> spirv_binary;
      spirv_binary.resize(size);

      uint32_t* bytes;
      uint8_t* first;

      for (size_t i = 0; i < size; ++i)
      {
        bytes = &spirv_binary.at(i);
        first = reinterpret_cast<uint8_t*>(bytes);

        for (size_t j = 0; j < 4; ++j)
        {
          first[j] = spirv[i * 4 + j];
        }
      }

      spirv_cross::CompilerGLSL glsl(spirv_binary);
      spirv_cross::CompilerHLSL hlsl(spirv_binary);

      spirv_cross::CompilerGLSL::Options glsl_op;
      glsl_op.version = kGLSLVersion_;
      glsl.set_common_options(glsl_op);

      spirv_cross::CompilerHLSL::Options hlsl_op;
      hlsl_op.shader_model = kShaderModel_;
      hlsl.set_hlsl_options(hlsl_op);

      std::string glsl_source = glsl.compile();
      std::string hlsl_source = hlsl.compile();

      if (glsl_source.size() == 0 || hlsl_source.size() == 0)
      {
        return false;
      }

      if (data != nullptr)
      {
        data->hlsl = hlsl_source.c_str();
        data->glsl = glsl_source.c_str();
      }

      return true;
    }
  }
}