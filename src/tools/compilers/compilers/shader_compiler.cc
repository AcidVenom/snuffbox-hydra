#include "tools/compilers/compilers/shader_compiler.h"

#include "tools/compilers/utils/glslang.h"

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

      if (AllocateSourceFile(spirv.data(), spirv.size(), &fd) == false)
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