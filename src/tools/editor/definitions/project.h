#pragma once

namespace snuffbox
{
  namespace editor
  {
    /**
    * @brief Contains definitions for the different environment variables
    *
    * @author Daniel Konings
    */
    class Project
    {
    public:

      static const char* kBuildFolder; //!< The build folder
      static const char* kAssetFolder; //!< The asset folder
    };
  }
}