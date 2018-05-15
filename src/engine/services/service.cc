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
    void IService::RegisterCVars(CVarService* cvar)
    {

    }

    //--------------------------------------------------------------------------
    const foundation::String& IService::name() const
    {
      return name_;
    }

    //--------------------------------------------------------------------------
    uint32_t IService::get_next_type()
    {
      static uint32_t uuid = 0;
      return uuid++;
    }

    //--------------------------------------------------------------------------
    IService::~IService()
    {

    }
  }
}