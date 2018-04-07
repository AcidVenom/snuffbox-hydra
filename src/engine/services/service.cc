#include "engine/services/service.h"

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    IService::IService(const char* name) :
      name_(name)
    {

    }

    //--------------------------------------------------------------------------
    uint32_t IService::get_next_type()
    {
      static uint32_t uuid = 0;
      return uuid++;
    }
  }
}