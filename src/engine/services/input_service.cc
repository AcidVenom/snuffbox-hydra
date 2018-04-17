#include "engine/services/input_service.h"
#include "engine/input/input_filter.h"

#include "engine/auxiliary/debug.h"

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    InputService::InputService() :
      ServiceBase("InputService")
    {

    }

    //--------------------------------------------------------------------------
    foundation::ErrorCodes InputService::OnInitialize(Application& app)
    {
      return foundation::ErrorCodes::kSuccess;
    }

    //--------------------------------------------------------------------------
    void InputService::OnShutdown(Application& app)
    {

    }

    //--------------------------------------------------------------------------
    void InputService::RegisterInputFilter(IInputFilter* filter)
    {
      if (filter == nullptr)
      {
        return;
      }

      for (size_t i = 0; i < filters_.size(); ++i)
      {
        if (filters_.at(i) == filter)
        {
          Debug::LogVerbosity<1>(
            foundation::LogSeverity::kWarning,
            "Attempted to register a duplicate input filter");

          return;
        }
      }

      filters_.push_back(filter);
    }

    //--------------------------------------------------------------------------
    void InputService::RemoveInputFilter(IInputFilter* filter)
    {
      for (size_t i = 0; i < filters_.size(); ++i)
      {
        if (filters_.at(i) == filter)
        {
          filters_.erase(filters_.begin() + i);
          break;
        }
      }
    }
  }
}