#include "engine/cvar/cvar_service.h"

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    CVarService::CVarService() :
      ServiceBase<CVarService>("CVarService")
    {

    }

    //--------------------------------------------------------------------------
    void CVarService::RegisterFromCLI(const CommandLineParser::CLI& cli)
    {
      static const char* default_description = "<Set from CLI>";
      static CVar<bool> test_bool = CVar<bool>("test_bool", "", false);
      static CVar<double> test_double = CVar<double>("test_double", "", 0.0);

      CVarValue* v = nullptr;
      const char* value, *cname;

      for (
        CommandLineParser::CLI::const_iterator it = cli.begin();
        it != cli.end();
        ++it)
      {
        const foundation::String& name = it->first;

        if ((v = Get(name)) == nullptr)
        {
          value = it->second.c_str();
          cname = name.c_str();

          if (test_bool.ParseFrom(value) == true)
          {
            Register(cname, default_description, test_bool.value());
          }
          else if (test_double.ParseFrom(value) == true)
          {
            Register(cname, default_description, test_double.value());
          }
          else
          {
            Register(cname, default_description, foundation::String(value));
          }
          
          continue;
        }

        v->Set(it->second.c_str());
      }
    }

    //--------------------------------------------------------------------------
    CVarValue* CVarService::Get(const foundation::String& name) const
    {
      CVarMap::const_iterator it = registered_.find(name);

      if (it == registered_.end())
      {
        return nullptr;
      }

      return it->second.get();
    }

    //--------------------------------------------------------------------------
    bool CVarService::Exists(const foundation::String& name) const
    {
      return registered_.find(name) != registered_.end();
    }

    //--------------------------------------------------------------------------
    void CVarService::OnInitialize(Application& app)
    {
    
    }

    //--------------------------------------------------------------------------
    void CVarService::OnShutdown(Application& app)
    {
      registered_.clear();
    }
  }
}