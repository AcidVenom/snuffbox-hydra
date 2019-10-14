#include "tools/builder/threading/build_scheduler.h"
#include "tools/builder/builder.h"

#include <foundation/auxiliary/logger.h>

#include <thread>

namespace snuffbox
{
  namespace builder
  {
    //--------------------------------------------------------------------------
    BuildScheduler::BuildScheduler() :
      was_building_(false),
      build_count_(0)
    {
      size_t count = std::thread::hardware_concurrency();
      jobs_.resize(count);

      foundation::IAllocator* alloc = &foundation::Memory::default_allocator();
      for (size_t i = 0; i < count; ++i)
      {
        jobs_.at(i) = foundation::Memory::Construct<BuildJob>(alloc);
      }
    }

    //--------------------------------------------------------------------------
    void BuildScheduler::Queue(const BuildItem& item)
    {
      std::lock_guard<std::mutex> lock(queue_mutex_);
      queue_.push(item);

      if (was_building_ == false)
      {
        build_count_ = 0;
        was_building_ = true;
      }

      ++build_count_;
    }

    //--------------------------------------------------------------------------
    void BuildScheduler::IdleNotification(Builder* builder)
    {
      Flush();
      WriteCompiled(builder);
      UpdateBuildStatus();
    }

    //--------------------------------------------------------------------------
    bool BuildScheduler::IsBuilding() const
    {
      BuildJob* job = nullptr;
      bool compiling = false;

      for (size_t i = 0; i < jobs_.size(); ++i)
      {
        job = jobs_.at(i);
        if (job->ready() == false || job->has_data() == true)
        {
          compiling = true;
          break;
        }
      }

      return compiling == true || queue_.empty() == false;
    }

    //--------------------------------------------------------------------------
    void BuildScheduler::Flush()
    {
      std::lock_guard<std::mutex> lock(queue_mutex_);
      size_t progress;

      while (queue_.empty() == false)
      {
        const BuildItem& top = queue_.top();

        if (ScheduleJob(top) == true)
        {
          progress = build_count_ - queue_.size() + 1;

          foundation::Logger::LogVerbosity<1>(
            foundation::LogChannel::kBuilder,
            foundation::LogSeverity::kInfo,
            "[{0}/{1}] {2}",
            progress,
            build_count_,
            top.relative);

          queue_.pop();
          continue;
        }

        break;
      }
    }

    //--------------------------------------------------------------------------
    void BuildScheduler::WriteCompiled(Builder* builder)
    {
      BuildJob* job = nullptr;

      for (size_t i = 0; i < jobs_.size(); ++i)
      {
        job = jobs_.at(i);

        if (job->has_data() == false)
        {
          continue;
        }

        const BuildJob::Result& result = job->SyncData();
        const BuildItem& item = job->current_item();

        if (result.success == false)
        {
          foundation::Logger::LogVerbosity<1>(
            foundation::LogChannel::kBuilder,
            foundation::LogSeverity::kError,
            "{0}> ({1}) Error:\n\t{2}",
            i,
            item.relative,
            result.error);

          continue;
        }

        builder->Write(item, result.buffer, result.length);

        foundation::Logger::LogVerbosity<3>(
          foundation::LogChannel::kBuilder,
          foundation::LogSeverity::kSuccess,
          "{0}> Built '{1}'",
          i,
          item.relative);
      }
    }

    //--------------------------------------------------------------------------
    bool BuildScheduler::ScheduleJob(const BuildItem& item)
    {
      BuildJob* job = nullptr;

      for (size_t i = 0; i < jobs_.size(); ++i)
      {
        job = jobs_.at(i);

        if (job->ready() == true && job->has_data() == false)
        {
          if (job->Compile(item) == false)
          {
            foundation::Logger::LogVerbosity<1>(
              foundation::LogChannel::kBuilder,
              foundation::LogSeverity::kError,
              "Could not start compilation on build job {0}",
              i);

            continue;
          }

          return true;
        }
      }

      return false;
    }

    //--------------------------------------------------------------------------
    void BuildScheduler::UpdateBuildStatus()
    {
      if (
        was_building_ == true &&
        IsBuilding() == false)
      {
        was_building_ = false;

        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kBuilder,
          foundation::LogSeverity::kSuccess,
          "Finished building {0} item(s)",
          build_count_);
      }
    }

    //--------------------------------------------------------------------------
    void BuildScheduler::Stop()
    {
      bool ready = false;
      BuildJob* current = nullptr;

      while (ready == false)
      {
        ready = true;

        for (size_t i = 0; i < jobs_.size(); ++i)
        {
          current = jobs_.at(i);
          if (current != nullptr)
          {
            if (current->ready() == false)
            {
              ready = false;
              break;
            }

            foundation::Memory::Destruct(current);
            jobs_.at(i) = nullptr;
          }
        }
      }

      jobs_.clear();
    }

    //--------------------------------------------------------------------------
    BuildScheduler::~BuildScheduler()
    {
      Stop();
    }
  }
}