#include "engine/services/cvar_service.h"

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    const size_t CVarService::kLogPadding_ = 20;

    //--------------------------------------------------------------------------
    CVarService::CVarService() :
      ServiceBase<CVarService>("CVarService")
    {

    }

    //--------------------------------------------------------------------------
    void CVarService::RegisterFromCLI(const CLI& cli)
    {
      static const char* default_description = "<Set from CLI>";
      static CVar<bool> test_bool = CVar<bool>("test_bool", "", false);
      static CVar<double> test_double = CVar<double>("test_double", "", 0.0);

      CVarValue* v = nullptr;
      const char* value, *cname;

      for (
        CLI::const_iterator it = cli.begin();
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
    void CVarService::ExecuteCommand(const CLICommand& cmd)
    {
      if (cmd.type == CLICommandType::kHelp)
      {
        return;
      }

      CLI cli;

      switch (cmd.type)
      {
      case CLICommandType::kSet:
        cli.emplace(eastl::pair<foundation::String, foundation::String>
        { 
          cmd.data[0], 
          cmd.data[1]
        });
        RegisterFromCLI(cli);
        break;

      case CLICommandType::kGet:
        LogValue(cmd.data[0]);
        break;

      case CLICommandType::kDesc:
        LogDescription(cmd.data[0]);
        break;

      case CLICommandType::kShowAll:
        LogAll();
        break;
      }
    }

    //--------------------------------------------------------------------------
    void CVarService::LogValue(const CVarValue* value) const
    {
      Debug::LogVerbosity<1>(
        foundation::LogSeverity::kDebug,
        value->ToString().c_str()
        );
    }

    //--------------------------------------------------------------------------
    void CVarService::LogValue(const foundation::String& name) const
    {
      CVarValue* v;
      if ((v = Get(name)) == nullptr)
      {
        Debug::LogVerbosity<1>(foundation::LogSeverity::kDebug, "<undefined>");
        return;
      }

      LogValue(v);
    }

    //--------------------------------------------------------------------------
    void CVarService::LogDescription(const CVarValue* value) const
    {
      Debug::LogVerbosity<1>(
        foundation::LogSeverity::kDebug,
        value->description().c_str()
        );
    }

    //--------------------------------------------------------------------------
    void CVarService::LogDescription(const foundation::String& name) const
    {
      CVarValue* v;
      if ((v = Get(name)) == nullptr)
      {
        Debug::LogVerbosity<1>(foundation::LogSeverity::kDebug, "<undefined>");
        return;
      }

      LogDescription(v);
    }

    //--------------------------------------------------------------------------
    void CVarService::LogAll() const
    {
      foundation::String values = "\n\n";

      for (
        CVarMap::const_iterator it = registered_.begin();
        it != registered_.end();
        ++it)
      {
        values += it->first;

        size_t len = it->first.size();

        for (size_t i = 0; i < kLogPadding_ - len; ++i)
        {
          values += " ";
        }

        values += 
          " = " +
          it->second->ToString() + " " +
          '\"' + it->second->description() + "\"\n";
      }

      Debug::LogVerbosity<1>(foundation::LogSeverity::kDebug, values.c_str());
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