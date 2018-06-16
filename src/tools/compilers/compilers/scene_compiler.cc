#include "tools/compilers/compilers/scene_compiler.h"

namespace snuffbox
{
  namespace compilers
  {
    //--------------------------------------------------------------------------
    SceneCompiler::SceneCompiler()
    {

    }

    //--------------------------------------------------------------------------
    bool SceneCompiler::CompileImpl(foundation::File& file)
    {
      SourceFileData fd;
      fd.magic = FileHeaderMagic::kScene;

      if (ReadSourceFile(file, &fd) == false)
      {
        return false;
      }

      SetData(fd.data, fd.total_size);

      return true;
    }

    //--------------------------------------------------------------------------
    bool SceneCompiler::DecompileImpl(foundation::File& file)
    {
      BuildFileData fd;
      fd.magic = FileHeaderMagic::kScene;

      if (ReadBuildFile(file, &fd) == false)
      {
        return false;
      }

      SetData(fd.block, fd.length);

      return true;
    }
  }
}