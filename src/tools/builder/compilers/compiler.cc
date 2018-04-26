#include "tools/builder/compilers/compiler.h"

#include <foundation/memory/memory.h>

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
      out_extension_(out_extension),
      error_(""),
      data_(nullptr),
      size_(0)
    {
      assert(supported_extensions_.size() > 0);
      assert(out_extension_.size() > 0);
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
    bool ICompiler::IsSupported(
      const foundation::Path& path, 
      bool compile) const
    {
      if (path.is_directory() == true)
      {
        return false;
      }

      const foundation::String& ext = path.extension();

      if (compile == false)
      {
        return ext == out_extension_;
      }

      for (size_t i = 0; i < supported_extensions_.size(); ++i)
      {
        if (ext == supported_extensions_.at(i))
        {
          return true;
        }
      }

      return false;
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

      if (IsSupported(path, compile) == false)
      {
        set_error(
          "Path for compilation is invalid\nAre you sure the file\
          extension is supported and the path is not a directory?");

        return false;
      }

      if (file->Open(path, foundation::FileFlags::kRead) == false)
      {
        set_error("Could not open file for reading");

        return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------
    void ICompiler::SetData(uint8_t* data, size_t size)
    {
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