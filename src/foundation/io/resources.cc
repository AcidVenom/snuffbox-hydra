#include "foundation/io/resources.h"

namespace snuffbox
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    Map<String, Resources::ResourceData> Resources::resources_;

    //--------------------------------------------------------------------------
    void Resources::Register(
      const uint8_t* buffer, 
      size_t size, 
      const Path& path)
    {
      resources_.emplace(eastl::pair<String, ResourceData>
      {
        path.ToString(), ResourceData{ buffer, size }
      });
    }

    //--------------------------------------------------------------------------
    void Resources::Shutdown()
    {
      resources_.clear();
    }

    //--------------------------------------------------------------------------
    const Resources::ResourceData* Resources::GetResource(const Path& path)
    {
      Map<String, ResourceData>::const_iterator it = 
        resources_.find(path.ToString());

      if (it == resources_.end())
      {
        return nullptr;
      }

      return &it->second;
    }
  }
}