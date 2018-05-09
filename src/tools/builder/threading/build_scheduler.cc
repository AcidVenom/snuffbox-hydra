#include "tools/builder/threading/build_scheduler.h"
#include "tools/builder/builder.h"

#include <foundation/auxiliary/logger.h>

#include <thread>

namespace snuffbox
{
  namespace builder
  {
    //--------------------------------------------------------------------------
    BuildScheduler::BuildScheduler()
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
    }

    //--------------------------------------------------------------------------
    void BuildScheduler::IdleNotification(Builder* builder)
    {
      Flush();
      WriteCompiled(builder);
    }

    //--------------------------------------------------------------------------
    void BuildScheduler::Flush()
    {
      std::lock_guard<std::mutex> lock(queue_mutex_);
      size_t id;

      while (queue_.empty() == false)
      {
        const BuildItem& top = queue_.top();

        if (ScheduleJob(top, &id) == true)
        {
          foundation::Logger::LogVerbosity<1>(
            foundation::LogChannel::kBuilder,
            foundation::LogSeverity::kInfo,
            "{0}> Building '{1}'",
            id,
            top.in);

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

        if (result.success == false)
        {
          continue;
        }

        builder->Write(job->current_item(), result.buffer, result.length);
      }
    }

    //--------------------------------------------------------------------------
    bool BuildScheduler::ScheduleJob(const BuildItem& item, size_t* id)
    {
      BuildJob* job = nullptr;

      for (size_t i = 0; i < jobs_.size(); ++i)
      {
        job = jobs_.at(i);

        if (job->ready() == true && job->has_data() == false)
        {
          if (id != nullptr)
          {
            *id = i;
          }

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