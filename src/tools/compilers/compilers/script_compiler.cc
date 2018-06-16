#include "tools/compilers/compilers/script_compiler.h"

#include <foundation/memory/memory.h>
#include <foundation/encryption/rc4.h>

namespace snuffbox
{
  namespace compilers
  {
    //--------------------------------------------------------------------------
    ScriptCompiler::ScriptCompiler()
    {

    }

    //--------------------------------------------------------------------------
    bool ScriptCompiler::CompileImpl(foundation::File& file)
    {
      SourceFileData fd;
      fd.magic = FileHeaderMagic::kScript;

      if (ReadSourceFile(file, &fd) == false)
      {
        return false;
      }

      foundation::RC4 rc4;
      rc4.Encrypt(reinterpret_cast<int8_t*>(fd.block), fd.length);

      SetData(fd.data, fd.total_size);

      return true;
    }

    //--------------------------------------------------------------------------
    bool ScriptCompiler::DecompileImpl(foundation::File& file)
    {
      BuildFileData fd;
      fd.magic = FileHeaderMagic::kScript;

      if (ReadBuildFile(file, &fd) == false)
      {
        return false;
      }

      foundation::RC4 rc4;
      rc4.Decrypt(reinterpret_cast<int8_t*>(fd.block), fd.length);

      SetData(reinterpret_cast<uint8_t*>(fd.block), fd.length);

      return true;
    }
  }
}