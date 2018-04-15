#pragma once

#include <foundation/containers/map.h>
#include <foundation/containers/string.h>

namespace snuffbox
{
  namespace engine
  {
    /**
    * @brief The different command types that exist
    */
    enum class CLICommandType
    {
      kHelp, //!< "help"
      kSet, //!< "set <name> <value>"
      kGet, //!< "get <name>"
      kDesc, //!< "desc <name>"
      kShowAll //!< "show_all"
    };

    /**
    * @brief Used to store command input from the console
    *
    * @author Daniel Konings
    */
    struct CLICommand
    {
      CLICommandType type; //!< The type of the command
      foundation::String data[2]; //!< The data
    };

    /**
    * @brief A container to store the CLI definitions
    */
    using CLI = foundation::Map<foundation::String, foundation::String>;
  }
}