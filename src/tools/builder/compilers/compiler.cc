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
      if (IsSupported(path, true) == false)
      {
        set_error(
          "Path for the compilation is invalid\nAre you sure the file\
          extension is supported and the path is not a directory?");

        return false;
      }

      return CompileImpl(path);
    }

    //--------------------------------------------------------------------------
    bool ICompiler::Decompile(const foundation::Path& path)
    {
      if (IsSupported(path, false) == false)
      {
        set_error(
          "Path for the decompilation is invalid\nAre you sure the file\
          extension is supported and the path is not a directory?");

        return false;
      }

      return CompileImpl(path);
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
    void ICompiler::set_error(const foundation::String& error)
    {
      error_ = error;
    }
  }
}