#include "foundation/auxiliary/timer.h"
#include "foundation/auxiliary/logger.h"

namespace snuffbox
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    Timer::Timer(const String& name) :
      name_(name),
      paused_(true),
      stopped_(false),
      elapsed_(0.0f),
      prev_elapsed_(0.0f),
      current_(std::chrono::high_resolution_clock::now())
    {

    }

    //--------------------------------------------------------------------------
    void Timer::Start()
    {
      if (paused_ == false)
      {
        return;
      }

      MoveCurrent();
      
      if (stopped_ == true)
      {
        prev_elapsed_ = 0.0f;
        stopped_ = false;
      }

      paused_ = false;
    }

    //--------------------------------------------------------------------------
    void Timer::Stop()
    {
      Pause();
      stopped_ = true;
    }

    //--------------------------------------------------------------------------
    void Timer::Pause()
    {
      Update();
      prev_elapsed_ = elapsed_;
      paused_ = true;
    }

    //--------------------------------------------------------------------------
    float Timer::Elapsed(TimeUnits unit, bool print)
    {
      static const float one_over_60 = 1.0f / 60.0f;

      Update();

      float elapsed = 0.0f;

      switch (unit)
      {

      case TimeUnits::kMillisecond:
        elapsed = elapsed_;
        break;

      case TimeUnits::kSecond:
        elapsed = elapsed_ * 1e-3f;
        break;

      case TimeUnits::kMinute:
        elapsed = elapsed_ * 1e-3f * one_over_60;
        break;

      }

      if (print == true)
      {
        String metric = "ms";

        switch (unit)
        {
        case TimeUnits::kMillisecond:
          metric = "ms";
          break;

        case TimeUnits::kSecond:
          metric = "second(s)";
          break;

        case TimeUnits::kMinute:
          metric = "minute(s)";
          break;
        }

        Logger::LogVerbosity<2>(
          LogChannel::kUnspecified,
          LogSeverity::kDebug,
          "Timer '{0}' ran for {1} {2}",
          name_,
          elapsed,
          metric);
      }

      return elapsed;
    }

    //--------------------------------------------------------------------------
    const String& Timer::name() const
    {
      return name_;
    }

    //--------------------------------------------------------------------------
    void Timer::Update()
    {
      if (paused_ == true)
      {
        elapsed_ = prev_elapsed_;
        return;
      }

      std::chrono::high_resolution_clock::time_point now = 
        std::chrono::high_resolution_clock::now();

      elapsed_ = 
        prev_elapsed_ +
        std::chrono::duration<float, std::milli>(now - current_).count();
    }

    //--------------------------------------------------------------------------
    void Timer::MoveCurrent()
    {
      current_ = std::chrono::high_resolution_clock::now();
    }
  }
}