#include "tools/compilers/scene_compiler.h"

namespace snuffbox
{
  namespace compilers
  {
    //--------------------------------------------------------------------------
    bool SceneCompiler::CompileImpl(foundation::File& file)
    {
      return true;
    }

    //--------------------------------------------------------------------------
    bool SceneCompiler::DecompileImpl(foundation::File& file)
    {
      return false;
    }
  }
}