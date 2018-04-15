#pragma once

#include "engine/definitions/cli.h"

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

      /**
      * @brief Parses a command that was input from the console
      *
      * @param[in] input The command string to parse
      * @param[out] command The parse command
      *
      * @return Was the input command parsed succesfully?
      */
      static bool ParseInput(const char* input, CLICommand* command);

    protected:

      /**
      * @brief Throws an error when the command line was not properly parsed
      *
      * @param[in] message The message to show
      *
      * @remarks This prints the CommandLineParser::Usage as well
      */
      static void Error(const char* message);

      /**
      * @return The usage text for the console input
      *
      * @remarks This text is shown whenever there is a parsing error or
      *          when the user requests "help"
      */
      static const char* Usage();
    };
  }
}