#include "tools/builder/threading/build_job.h"
#include "tools/builder/compilers/script_compiler.h"

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
        ready_ = true;

        if (result_.success == false)
        {
          foundation::Logger::LogVerbosity<1>(
            foundation::LogChannel::kBuilder,
            foundation::LogSeverity::kError,
            "Could not compile '{0}'\n\t{1}",
            current_item_.in,
            compiler_->error());

          return;
        }

        result_.buffer = compiler_->Data(&result_.length);

        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kBuilder,
          foundation::LogSeverity::kSuccess,
          "Succesfully compiled '{0}'",
          current_item_.in);

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
    foundation::SharedPtr<ICompiler> BuildJob::CreateCompiler(AssetTypes type)
    {
      foundation::SharedPtr<ICompiler> ptr = nullptr;
      foundation::IAllocator* alloc = &foundation::Memory::default_allocator();

      switch (type)
      {
      case AssetTypes::kScript:
        ptr = foundation::Memory::ConstructShared<ScriptCompiler>(alloc);
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