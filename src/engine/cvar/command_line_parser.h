#pragma once

#include <foundation/containers/map.h>
#include <foundation/containers/string.h>

namespace snuffbox
{
  namespace engine
  {
    /**
    * @brief Parses the command line passed into the application's main
    *        entry point
    *
    * @author Daniel Konings
    */
    class CommandLineParser
    {

    public:

      /**
      * @brief A container to store the CLI definitions
      */
      using CLI = foundation::Map<foundation::String, foundation::String>;

      /**
      * @brief Parses the command line and stores the respective string
      *        values.
      *
      * All commands that should be registered need to start with '-'.
      * The values are then followed by a space, where their string value
      * will be. This parser doesn't do any conversions to CVars, that is
      * handled separately in the CVarService.
      *
      * @param[in] argc The number of arguments passed into the application
      * @param[in] argv The arguments passed into the application
      */
      static CLI Parse(int argc, char** argv);
    };
  }
}