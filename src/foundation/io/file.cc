#include "foundation/io/file.h"
#include "foundation/memory/memory.h"
#include "foundation/auxiliary/pointer_math.h"
#include "foundation/io/resources.h"

#include <cstdio>

#ifdef SNUFF_LINUX
#include <sys/stat.h>
#endif

namespace snuffbox
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    File::File() :
      is_ok_(false),
      virtual_buffer_(nullptr),
      buffer_(nullptr),
      length_(0)
    {

    }

    //--------------------------------------------------------------------------
    File::File(const Path& path, FileOpenMode mode) :
      is_ok_(false),
      virtual_buffer_(nullptr),
      buffer_(nullptr),
      length_(0)
    {
      Open(path, mode);
    }

    //--------------------------------------------------------------------------
    bool File::Open(const Path& path, FileOpenMode mode)
    {
      if (path.is_virtual() == true)
      {
        is_ok_ = OpenVirtual(path);
        return is_ok_;
      }

      is_ok_ = OpenFile(path, mode);

      return is_ok_;
    }

    //--------------------------------------------------------------------------
    void File::Close()
    {
      if (is_ok_ == false)
      {
        return;
      }

      if (buffer_ != nullptr)
      {
        Memory::Deallocate(buffer_);
        buffer_ = nullptr;
      }

      stream_.close();
      is_ok_ = false;
    }

    //--------------------------------------------------------------------------
    const uint8_t* File::ReadBuffer(size_t* length, bool is_string)
    {
      if (length == nullptr)
      {
        return nullptr;
      }

      if (is_ok_ == false)
      {
        *length = 0;
        return nullptr;
      }

      if (virtual_buffer_ != nullptr)
      {
        *length = length_;
        return virtual_buffer_;
      }

      size_t len = length_ + (is_string == true ? 1 : 0);

      if (buffer_ == nullptr)
      {
        buffer_ = reinterpret_cast<uint8_t*>(Memory::Allocate(len));

        stream_.seekg(std::ios_base::beg);
        stream_.read(reinterpret_cast<char*>(buffer_), length_);
        
        if (is_string == true)
        {
          void* block = PointerMath::Offset(buffer_, length_);
          memset(block, '\0', sizeof(uint8_t));
        }
      }

      *length = len;
      return buffer_;
    }

    //--------------------------------------------------------------------------
    void File::Write(const uint8_t* buffer, size_t size)
    {
      if (is_ok_ == false)
      {
        return;
      }

      stream_.write(reinterpret_cast<const char*>(buffer), size);
      stream_.flush();
    }

    //--------------------------------------------------------------------------
    void File::Remove(const Path& path)
    {
      remove(path.ToString().c_str());
    }

    //--------------------------------------------------------------------------
    bool File::Exists(const Path& path)
    {
      std::ifstream f(path.ToString().c_str(), std::ios::in);
      return f.good();
    }

    //--------------------------------------------------------------------------
    bool File::is_ok() const
    {
      return is_ok_;
    }

    //--------------------------------------------------------------------------
    size_t File::length() const
    {
      return length_;
    }

    //--------------------------------------------------------------------------
    const time_t& File::last_modified() const
    {
      return last_modified_;
    }

    //--------------------------------------------------------------------------
    File::~File()
    {
      Close();
    }

    //--------------------------------------------------------------------------
    bool File::OpenVirtual(const Path& path)
    {
      const Resources::ResourceData* d = 
        Resources::GetResource(path.StripPath(Path::kVirtualPrefix));

      if (d == nullptr)
      {
        length_ = 0;
        return false;
      }

      virtual_buffer_ = d->buffer;
      length_ = d->size;

      return true;
    }

    //--------------------------------------------------------------------------
    bool File::OpenFile(const Path& path, FileOpenMode mode)
    {
      const char* cpath = path.ToString().c_str();

      stream_ = std::fstream(
        cpath,
        FileFlagsToOpenMode(static_cast<FileFlags>(mode)));

      if (stream_.is_open() == false)
      {
        length_ = 0;
        return false;
      }

      length_ = stream_.tellg();
      stream_.seekg(std::ios_base::beg);

      struct stat attributes;
      stat(cpath, &attributes);

      last_modified_ = attributes.st_mtime;

      return true;
    }

    //--------------------------------------------------------------------------
    std::ios::openmode File::FileFlagsToOpenMode(FileFlags flags)
    {
      std::ios::openmode mode = static_cast<std::ios::openmode>(0);

      if ((flags & FileFlags::kRead) == FileFlags::kRead)
      {
        mode |= std::ios::in;
      }

      if ((flags & FileFlags::kWrite) == FileFlags::kWrite)
      {
        mode |= std::ios::out | std::ios::trunc;
      }

      mode |= std::ios::binary | std::ios::ate;

      return mode;
    }
  }
}
