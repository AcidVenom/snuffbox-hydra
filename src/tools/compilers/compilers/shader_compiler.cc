#include "tools/compilers/compilers/shader_compiler.h"

#include "tools/compilers/utils/glslang.h"

#ifdef SNUFF_WIN32
#include "tools/compilers/utils/hlsl_compiler.h"
#endif

#include <foundation/auxiliary/logger.h>

namespace snuffbox
{
  namespace compilers
  {
    //--------------------------------------------------------------------------
    ShaderCompiler::ShaderCompiler(compilers::AssetTypes type) :
      type_(type)
    {

    }

    //--------------------------------------------------------------------------
    bool ShaderCompiler::CompileImpl(foundation::File& file)
    {
      SourceFileData fd;
      fd.magic = ShaderTypeToMagic(type_);

      if (fd.magic == FileHeaderMagic::kUnknown)
      {
        set_error("Unknown shader type");
        return false;
      }

      size_t len;
      const char* block = reinterpret_cast<const char*>(file.ReadBuffer(&len));
      foundation::String hlsl(block, len);

      Glslang glslang;
      foundation::Vector<uint8_t> spirv;
      if (glslang.Compile(hlsl, file.path(), type_, &spirv) == false)
      {
        set_error(glslang.error());
        return false;
      }

      bool has_hlsl = false;
      const uint8_t* hlsl_buffer = nullptr;
      size_t hlsl_buffer_size = 0;

#ifdef SNUFF_WIN32
      HLSLCompiler hlsl_c;
      if (hlsl_c.Compile(hlsl, type_) == false)
      {
        set_error(hlsl_c.error());
        return false;
      }

      has_hlsl = true;
      hlsl_buffer = hlsl_c.GetBuffer(&hlsl_buffer_size);
#endif

      foundation::Vector<uint8_t> buffer;

      const size_t header_size = sizeof(ShaderHeader);
      size_t spirv_size = spirv.size();
      size_t hlsl_size = has_hlsl == true ? hlsl_buffer_size : 0;

      size_t block_size = header_size + spirv_size + hlsl_size;

      buffer.resize(block_size);
      
      ShaderHeader header;
      header.has_hlsl = has_hlsl;
      header.hlsl_size = hlsl_size;
      header.hlsl_offset = header_size;
      header.spirv_size = spirv_size;
      header.spirv_offset = header.hlsl_offset + hlsl_size;

      memcpy(&buffer.at(0), &header, header_size);

      if (header.has_hlsl == true)
      {
        memcpy(&buffer.at(header_size), hlsl_buffer, hlsl_size);
      }

      memcpy(&buffer.at(header.spirv_offset), spirv.data(), spirv_size);

      if (AllocateSourceFile(buffer.data(), buffer.size(), &fd) == false)
      {
        return false;
      }

      SetData(fd.data, fd.total_size);

      return true;
    }

    //--------------------------------------------------------------------------
    bool ShaderCompiler::DecompileImpl(foundation::File& file)
    {
      BuildFileData fd;
      fd.magic = ShaderTypeToMagic(type_);

      if (ReadBuildFile(file, &fd) == false)
      {
        return false;
      }

      const ShaderHeader* header = 
        reinterpret_cast<const ShaderHeader*>(fd.block);

      size_t offset = 0;
      size_t size = 0;

#if defined (SNUFF_D3D11) || defined (SNUFF_D3D12)

      if (header->has_hlsl == false)
      {
        set_error("Requested a HLSL shader but it was never built");
        return false;
      }

      size = header->hlsl_size;
      offset = header->hlsl_offset;

#elif defined (SNUFF_OGL)

      size = header->spirv_size;
      offset = header->spirv_offset;

#else
      static_assert(false, 
        "Unknown shader format requested, no supported renderer");
#endif

      SetData(reinterpret_cast<uint8_t*>(fd.block), size, offset);

      return true;
    }

    //--------------------------------------------------------------------------
    FileHeaderMagic ShaderCompiler::ShaderTypeToMagic(AssetTypes type)
    {
      switch (type)
      {
      case AssetTypes::kVertexShader:
        return FileHeaderMagic::kVertexShader;

      case AssetTypes::kPixelShader:
        return FileHeaderMagic::kPixelShader;

      case AssetTypes::kGeometryShader:
        return FileHeaderMagic::kGeometryShader;

      default:
        break;
      }

      return FileHeaderMagic::kUnknown;
    }
  }
}