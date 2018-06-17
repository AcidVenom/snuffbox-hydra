#include "tools/compilers/compilers/shader_compiler.h"

#include "tools/compilers/utils/glslang.h"
#include "tools/compilers/utils/spirv_cross.h"

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
      
      switch (type_)
      {
      case AssetTypes::kVertexShader:
        fd.magic = FileHeaderMagic::kVertexShader;
        break;

      case AssetTypes::kPixelShader:
        fd.magic = FileHeaderMagic::kPixelShader;
        break;

      case AssetTypes::kGeometryShader:
        fd.magic = FileHeaderMagic::kGeometryShader;
        break;

      default:
        return false;
      }

      if (type_ == AssetTypes::kGeometryShader)
      {
        set_error(
          "Currently,\
          due to SPIRV-Cross not being able to compile HLSL geometry shaders;\
          they are unimplemented");

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

      SPIRVCross spv_cross;
      SPIRVCross::ShaderData sh_data;
      if (spv_cross.Compile(
        spirv.data(), 
        spirv.size(), 
        type_, 
        &sh_data) == false)
      {
        set_error(spv_cross.error());
        return false;
      }

      foundation::Vector<uint8_t> buffer;

      const size_t header_size = sizeof(ShaderHeader);
      size_t glsl_size = sh_data.glsl.size();
      size_t hlsl_size = sh_data.has_hlsl == true ? sh_data.hlsl.size() : 0;

      size_t block_size = header_size + glsl_size + hlsl_size;

      buffer.resize(block_size);
      
      ShaderHeader header;
      header.has_hlsl = sh_data.has_hlsl;
      header.hlsl_size = hlsl_size;
      header.hlsl_offset = header_size;
      header.glsl_size = glsl_size;
      header.glsl_offset = header.hlsl_offset + hlsl_size;

      memcpy(&buffer.at(0), &header, header_size);

      if (header.has_hlsl == true)
      {
        memcpy(&buffer.at(header_size), sh_data.hlsl.data(), hlsl_size);
      }

      memcpy(&buffer.at(header.glsl_offset), sh_data.glsl.c_str(), glsl_size);

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
      return false;
    }
  }
}