#include "tools/compilers/compilers/material_compiler.h"

namespace snuffbox
{
  namespace compilers
  {
    //--------------------------------------------------------------------------
    MaterialCompiler::MaterialCompiler()
    {

    }

    //--------------------------------------------------------------------------
    bool MaterialCompiler::CompileImpl(foundation::File& file)
    {
      SourceFileData fd;
      fd.magic = FileHeaderMagic::kMaterial;

      if (AllocateSourceFile(file, &fd) == false)
      {
        return false;
      }

      SetData(fd.data, fd.total_size);

      return true;
    }

    //--------------------------------------------------------------------------
    bool MaterialCompiler::DecompileImpl(foundation::File& file)
    {
      BuildFileData fd;
      fd.magic = FileHeaderMagic::kMaterial;

      if (ReadBuildFile(file, &fd) == false)
      {
        return false;
      }

      SetData(fd.block, fd.length);

      return true;
    }
  }
}