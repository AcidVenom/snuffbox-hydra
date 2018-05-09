#include "tools/builder/compilers/script_compiler.h"

#include <foundation/memory/memory.h>
#include <foundation/encryption/rc4.h>

namespace snuffbox
{
  namespace builder
  {
    //--------------------------------------------------------------------------
    ScriptCompiler::ScriptCompiler()
    {

    }

    //--------------------------------------------------------------------------
    bool ScriptCompiler::CompileImpl(foundation::File& file)
    {
      size_t len, total_size;
      const uint8_t* buffer = file.ReadBuffer(&len);

      uint8_t* ptr = nullptr;

      uint8_t* block = AllocateWithMagic(
        FileHeaderMagic::kScript, 
        len, 
        &ptr, 
        &total_size);

      memcpy(ptr, buffer, len);

      foundation::RC4 rc4;
      rc4.Encrypt(reinterpret_cast<int8_t*>(ptr), len);

      SetData(block, total_size);

      return true;
    }

    //--------------------------------------------------------------------------
    bool ScriptCompiler::DecompileImpl(foundation::File& file)
    {
      size_t len, block_size;
      const void* buffer = file.ReadBuffer(&len);

      const uint8_t* data = nullptr;
      FileHeaderMagic magic = GetMagic(
          reinterpret_cast<const uint8_t*>(buffer), 
          len, 
          &data, 
          &block_size);

      if (magic != FileHeaderMagic::kScript)
      {
        set_error("File is not of a script type");
        return false;
      }

      void* block = foundation::Memory::Allocate(block_size);
      memcpy(block, data, block_size);

      foundation::RC4 rc4;

      rc4.Decrypt(reinterpret_cast<int8_t*>(block), block_size);

      SetData(reinterpret_cast<uint8_t*>(block), block_size);

      return true;
    }
  }
}