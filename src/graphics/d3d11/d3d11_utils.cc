#include "graphics/d3d11/d3d11_utils.h"

#include <foundation/auxiliary/logger.h>

namespace snuffbox
{
  namespace graphics
  {
    //--------------------------------------------------------------------------
    foundation::String D3D11Utils::HRToString(HRESULT err)
    {
      _com_error error(err);
      return error.ErrorMessage();
    }

    //--------------------------------------------------------------------------
    bool D3D11Utils::Check(HRESULT err)
    {
      if (err == S_OK)
      {
        return true;
      }

      foundation::Logger::LogVerbosity<2>(
        foundation::LogChannel::kEngine,
        foundation::LogSeverity::kError,
        "{0}",
        HRToString(err));

      return false;
    }
  }
}