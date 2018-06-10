#include "tools/builder/threading/build_job.h"
#include "tools/compilers/script_compiler.h"

#include <foundation/auxiliary/logger.h>

namespace snuffbox
{
  namespace builder
  {
    //--------------------------------------------------------------------------
    BuildJob::BuildJob() :
      ready_(true),
      has_data_(false),
      compiler_(nullptr)
    {

    }

    //--------------------------------------------------------------------------
    bool BuildJob::Compile(const BuildItem& item)
    {
      compiler_ = CreateCompiler(item.type);

      if (compiler_ == nullptr || ready_ == false || has_data_ == true)
      {
        return false;
      }

      ready_ = has_data_ = false;
      current_item_ = item;

      thread_ = std::thread([&]()
      {
        result_.success = compiler_->Compile(current_item_.in);
        result_.error = compiler_->error();
        result_.buffer = compiler_->Data(&result_.length);

        ready_ = true;
        has_data_ = true;
      });

      return true;
    }

    //--------------------------------------------------------------------------
    const BuildJob::Result& BuildJob::SyncData()
    {
      Join();

      has_data_ = false;
      return result_;
    }

    //--------------------------------------------------------------------------
    bool BuildJob::ready() const
    {
      return ready_;
    }

    //--------------------------------------------------------------------------
    bool BuildJob::has_data() const
    {
      return has_data_;
    }

    //--------------------------------------------------------------------------
    const BuildItem& BuildJob::current_item() const
    {
      return current_item_;
    }

    //--------------------------------------------------------------------------
    BuildJob::~BuildJob()
    {
      Join();
    }

    //--------------------------------------------------------------------------
    void BuildJob::Join()
    {
      if (thread_.joinable() == true)
      {
        thread_.join();
      }
    }

    //--------------------------------------------------------------------------
    foundation::SharedPtr<compilers::ICompiler> 
      BuildJob::CreateCompiler(compilers::AssetTypes type)
    {
      foundation::SharedPtr<compilers::ICompiler> ptr = nullptr;
      foundation::IAllocator* alloc = &foundation::Memory::default_allocator();

      switch (type)
      {
      case compilers::AssetTypes::kScript:
        ptr = foundation::Memory::ConstructShared<
          compilers::ScriptCompiler>(alloc);
        break;

      default:

        foundation::Logger::Assert(
          false, 
          "Unknown asset type to create a compiler with");
        break;
      }

      return ptr;
    }
  }
}