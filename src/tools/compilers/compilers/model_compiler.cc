#include "tools/compilers/compilers/model_compiler.h"

namespace snuffbox
{
  namespace compilers
  {
    //--------------------------------------------------------------------------
    ModelCompiler::ModelCompiler()
    {

    }

    //--------------------------------------------------------------------------
    bool ModelCompiler::CompileImpl(foundation::File& file)
    {
      return true;
    }

    //--------------------------------------------------------------------------
    bool ModelCompiler::DecompileImpl(foundation::File& file)
    {
      return true;
    }
  }
}