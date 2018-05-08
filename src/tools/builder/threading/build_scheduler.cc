#include "tools/builder/threading/build_scheduler.h"

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
    }

    //--------------------------------------------------------------------------
    void BuildScheduler::Queue(const BuildItem& item)
    {
      std::lock_guard<std::mutex> lock(queue_mutex_);
      queue_.push(item);
    }

    //--------------------------------------------------------------------------
    void BuildScheduler::IdleNotification()
    {
      Flush();
    }

    //--------------------------------------------------------------------------
    void BuildScheduler::Flush()
    {
      std::lock_guard<std::mutex> lock(queue_mutex_);

      while (queue_.empty() == false)
      {
        const BuildItem& top = queue_.back();

        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kBuilder,
          foundation::LogSeverity::kInfo,
          "Building '{0}'",
          top.in);

        queue_.pop();
      }
    }
  }
}