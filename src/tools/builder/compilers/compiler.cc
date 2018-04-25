#include "tools/builder/compilers/compiler.h"

#include <cassert>

namespace snuffbox
{
  namespace builder
  {
    //--------------------------------------------------------------------------
    ICompiler::ICompiler(
      const foundation::Vector<foundation::String>& supported_extensions,
      const foundation::String& out_extension)
      :
      supported_extensions_(supported_extensions),
      out_extension_(out_extension)
    {
      assert(supported_extensions_.size() > 0);
      assert(out_extension_.size() > 0);
    }

    //--------------------------------------------------------------------------

  }
}