#include "tools/builder/compilers/script_compiler.h"
#include "tools/builder/definitions/magic.h"

#include <foundation/memory/memory.h>
#include <foundation/auxiliary/pointer_math.h>
#include <foundation/encryption/rc4.h>

namespace snuffbox
{
  namespace builder
  {
    //--------------------------------------------------------------------------
    ScriptCompiler::ScriptCompiler() :
      ICompiler({ "js" }, "script")
    {

    }

    //--------------------------------------------------------------------------
    bool ScriptCompiler::CompileImpl(foundation::File& file)
    {
      size_t len;
      const uint8_t* buffer = file.ReadBuffer(&len);

      size_t header_size = sizeof(FileHeaderMagic);
      FileHeaderMagic magic = FileHeaderMagic::kScript;

      size_t total_size = len + header_size;

      void* block = foundation::Memory::Allocate(total_size);
      SetData(reinterpret_cast<uint8_t*>(block), total_size);

      memcpy(block, &magic, header_size);

      block = foundation::PointerMath::Offset(
        block, 
        static_cast<int64_t>(header_size));

      memcpy(block, buffer, len);

      foundation::RC4 rc4;

      rc4.Encrypt(reinterpret_cast<int8_t*>(block), len);

      return true;
    }

    //--------------------------------------------------------------------------
    bool ScriptCompiler::DecompileImpl(foundation::File& file)
    {
      size_t len;
      const void* buffer = file.ReadBuffer(&len);

      size_t header_size = sizeof(FileHeaderMagic);
      FileHeaderMagic magic = *reinterpret_cast<const FileHeaderMagic*>(buffer);

      if (magic != FileHeaderMagic::kScript)
      {
        set_error("File is not of a script type");
        return false;
      }

      buffer = foundation::PointerMath::Offset(buffer, header_size);

      size_t data_size = len - header_size;

      void* block = foundation::Memory::Allocate(data_size);
      memcpy(block, buffer, data_size);

      foundation::RC4 rc4;

      rc4.Decrypt(reinterpret_cast<int8_t*>(block), data_size);

      SetData(reinterpret_cast<uint8_t*>(block), data_size);

      return true;
    }
  }
}