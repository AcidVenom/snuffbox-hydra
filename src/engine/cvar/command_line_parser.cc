#include "engine/cvar/command_line_parser.h"
#include "engine/auxiliary/debug.h"

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    CommandLineParser::CLI CommandLineParser::Parse(int argc, char** argv)
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
  }
}