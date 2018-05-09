#include "tools/builder/compilers/compiler.h"

#include <foundation/memory/memory.h>
#include <foundation/auxiliary/logger.h>
#include <foundation/auxiliary/pointer_math.h>

#include <cassert>

namespace snuffbox
{
  namespace builder
  {
    //--------------------------------------------------------------------------
    ICompiler::ICompiler() :
      data_(nullptr)
    {

    }

    //--------------------------------------------------------------------------
    bool ICompiler::Compile(const foundation::Path& path)
    {
      Clear();

      foundation::File file;
      if (OpenFile(path, true, &file) == false)
      {
        return false;
      }

      return CompileImpl(file);
    }

    //--------------------------------------------------------------------------
    bool ICompiler::Decompile(const foundation::Path& path)
    {
      Clear();

      foundation::File file;
      if (OpenFile(path, false, &file) == false)
      {
        return false;
      }

      return DecompileImpl(file);
    }

    //--------------------------------------------------------------------------
    const uint8_t* ICompiler::Data(size_t* size) const
    {
      if (size != nullptr)
      {
        *size = data_ == nullptr ? 0 : size_;
      }

      return data_;
    }

    //--------------------------------------------------------------------------
    const foundation::String& ICompiler::error() const
    {
      return error_;
    }

    //--------------------------------------------------------------------------
    bool ICompiler::OpenFile(
      const foundation::Path& path,
      bool compile,
      foundation::File* file)
    {
      if (file == nullptr)
      {
        return false;
      }

      if (path.is_directory() == true)
      {
        set_error(
          "Path for compilation is invalid\nAre you sure the file\
           path is not a directory?");

        return false;
      }

      if (file->Open(path, foundation::FileFlags::kRead) == false)
      {
        set_error("Could not open file for reading during compilation");

        return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------
    void ICompiler::SetData(uint8_t* data, size_t size)
    {
      Clear();

      data_ = data;
      size_ = size;
    }

    //--------------------------------------------------------------------------
    void ICompiler::Clear()
    {
      if (data_ != nullptr)
      {
        foundation::Memory::Deallocate(data_);
        data_ = nullptr;
      }

      size_ = 0;
    }

    //--------------------------------------------------------------------------
    uint8_t* ICompiler::AllocateWithMagic(
      FileHeaderMagic magic,
      size_t size,
      uint8_t** block,
      size_t* total_size)
    {
      size_t header_size = sizeof(FileHeaderMagic);

      size_t new_size = size + header_size;

      if (total_size != nullptr)
      {
        *total_size = new_size;
      }

      void* ptr = foundation::Memory::Allocate(new_size);
      uint8_t* full_block = reinterpret_cast<uint8_t*>(ptr);

      memcpy(ptr, &magic, header_size);

      if (block != nullptr)
      {
        *block = foundation::PointerMath::Offset(
            static_cast<uint8_t*>(ptr), 
            static_cast<intptr_t>(header_size));
      }

      return full_block;
    }

    //--------------------------------------------------------------------------
    FileHeaderMagic ICompiler::GetMagic(
      const uint8_t* buffer, 
      size_t size,
      const uint8_t** block,
      size_t* block_size)
    {
      intptr_t header_size = static_cast<intptr_t>(sizeof(FileHeaderMagic));
      FileHeaderMagic magic = *reinterpret_cast<const FileHeaderMagic*>(buffer);

      if (block != nullptr)
      {
        *block = foundation::PointerMath::Offset(buffer, header_size);
      }

      if (block_size != nullptr)
      {
        *block_size = static_cast<size_t>(size) - header_size;
      }

      return magic;
    }

    //--------------------------------------------------------------------------
    void ICompiler::set_error(const foundation::String& error)
    {
      error_ = error;
    }

    //--------------------------------------------------------------------------
    ICompiler::~ICompiler()
    {
      Clear();
    }
  }
}