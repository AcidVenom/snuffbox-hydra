#include "engine/cvar/command_line_parser.h"
#include "engine/auxiliary/debug.h"

#include <cctype>

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    CLI CommandLineParser::Parse(int argc, char** argv)
    {
      CLI parsed;

      const char* arg = nullptr;
      const char* val = nullptr;

      for (int i = 1; i < argc; ++i)
      {
        arg = argv[i];

        if (strlen(arg) > 1 && arg[0] == '-')
        {
          ++arg;

          if (i + 1 < argc)
          {
            val = argv[i + 1];

            if (strlen(val) > 1 && val[0] == '-')
            {
              Debug::LogVerbosity<1>(
                foundation::LogSeverity::kWarning,
                "CLI: Unexpected value '{0}' for argument '{1}'",
                val,
                arg);

              continue;
            }

            parsed.emplace(eastl::pair<foundation::String, foundation::String>
            {
              arg, val
            });
          }
        }
      }

      return parsed;
    }

    //--------------------------------------------------------------------------
    bool CommandLineParser::ParseInput(const char* input, CLICommand* command)
    {
      foundation::String argv[3];

      for (int i = 0; i < 3; ++i)
      {
        argv[i] = "";
      }

      size_t len = strlen(input);
      int argc = 0;
      bool found_ws = false;
      char in_quotes = '\0';
      bool at_end = false;

      char c;
      for (size_t i = 0; i < len; ++i)
      {
        c = input[i];
        at_end = i == len - 1;

        if (c == '\"' || c == '\'')
        {
          if (in_quotes == '\0')
          {
            in_quotes = c;
            continue;
          }

          if (c == in_quotes)
          {
            in_quotes = '\0';

            if (at_end == false)
            {
              continue;
            }
          }
        }

        if (std::isspace(c) != 0 || at_end == true)
        {
          if (at_end == true && (c != '\'' && c != '\"') || in_quotes != '\0')
          {
            argv[argc] += c;
          }

          if ((found_ws == false || at_end == true) && in_quotes == '\0')
          {
            ++argc;

            if (argc > 3)
            {
              Error("Arguments with a length of more than 3 are not supported");
              return false;
            }

            found_ws = true;
          }

          continue;
        }

        argv[argc] += c;
        found_ws = false;
      }

      if (in_quotes != '\0')
      {
        Error("Couldn't find a matching quote to close a string");
        return false;
      }

      CLICommand cmd;

      if (argv[0] == "help" && argc == 1)
      {
        Debug::LogVerbosity<1>(
          foundation::LogSeverity::kInfo,
          "Usage:\n{0}\n",
          Usage());

        cmd.type = CLICommandType::kHelp;
      }
      else if (argv[0] == "set" && argc == 3)
      {
        cmd.type = CLICommandType::kSet;
        cmd.data[0] = argv[1];
        cmd.data[1] = argv[2];
      }
      else if (argv[0] == "get" && argc == 2)
      {
        cmd.type = CLICommandType::kGet;
        cmd.data[0] = argv[1];
      }
      else if (argv[0] == "desc" && argc == 2)
      {
        cmd.type = CLICommandType::kDesc;
        cmd.data[0] = argv[1];
      }
      else if (argv[0] == "show_all" && argc == 1)
      {
        cmd.type = CLICommandType::kShowAll;
      }
      else
      {
        Error("Invalid arguments passed into the command line");
        return false;
      }

      if (command != nullptr)
      {
        *command = cmd;
      }

      return true;
    }

    //--------------------------------------------------------------------------
    void CommandLineParser::Error(const char* message)
    {
      Debug::LogVerbosity<1>(
        foundation::LogSeverity::kError,
        "CLI: {0}\n\nUsage:\n{1}\n",
        foundation::String(message),
        foundation::String(Usage()));
    }

    //--------------------------------------------------------------------------
    const char* CommandLineParser::Usage()
    {
      return
        "\n\thelp\t\t\t-\tShows this dialog\
         \n\tset <name> <value>\t-\tSets a value of a CVar\
         \n\tget <name>\t\t-\tRetrieves the value of a CVar\
         \n\tdesc <name>\t\t-\tRetrieves the description of a CVar\
         \n\tshow_all\t\t-\tShows all currently registered CVars";
    }
  }
}