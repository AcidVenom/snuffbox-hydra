#include "engine/application/application.h"
#include "engine/auxiliary/debug.h"

#include "engine/services/cvar_service.h"

#include <cassert>
#include <iostream>

#include <scripting/scripting.h>

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    Application::Configuration 
      Application::Configuration::kDefaultConfiguration =
    {
      "Snuffbox",
      "0.0",
      1
    };

    //--------------------------------------------------------------------------
    Application* Application::instance_ = nullptr;

    //--------------------------------------------------------------------------
    Application::Application(
      int argc, 
      char** argv, 
      const Application::Configuration& config)
      :
      config_(config)
    {
      assert(
        instance_ == nullptr && 
        "There can only be one instance of a Snuffbox application");

      instance_ = this;

      if (argc > 1 && argv != nullptr)
      {
        cli_ = CommandLineParser::Parse(argc, argv);
      }
    }

    //--------------------------------------------------------------------------
    void Application::ApplyConfiguration()
    {
      foundation::Logger::SetVerbosity(config_.verbosity);

      Debug::LogVerbosity<1>(
        foundation::LogSeverity::kInfo,
        "Configuration:\n\
        \n\
        \tApplication name: {0}\n\
        \tVersion string: {1}\n\
        \tVerbosity: {2}\n\
        ",
        config_.application_name,
        config_.version_string,
        config_.verbosity);
    }

    //--------------------------------------------------------------------------
    foundation::ErrorCodes Application::Run()
    {
      ApplyConfiguration();

      Debug::LogVerbosity<2>(
        foundation::LogSeverity::kDebug,
        "Initializing the application"
        );

      Initialize();

      scripting::ScriptState state;
      state.Initialize();

      std::string input;
      while (input != "exit")
      {
        std::getline(std::cin, input);

        if (input.size() > 0 && input != "exit")
        {
          state.CompileFromSource("console", input.c_str(), true);
        }
      }

      Shutdown();
      state.Shutdown();

      return foundation::ErrorCodes::kSuccess;
    }

    //--------------------------------------------------------------------------
    void Application::Initialize()
    {
      CVarService* cvar = CreateService<CVarService>();

      InitializeServices();

      OnInitialize();
      cvar->RegisterFromCLI(cli_);
    }

    //--------------------------------------------------------------------------
    void Application::Update(float dt)
    {
      OnUpdate(dt);
    }

    //--------------------------------------------------------------------------
    void Application::FixedUpdate(float time_step)
    {
      OnFixedUpdate(time_step);
    }

    //--------------------------------------------------------------------------
    void Application::Render(float dt)
    {
      OnRender(dt);
    }

    //--------------------------------------------------------------------------
    void Application::Shutdown()
    {
      Debug::LogVerbosity<2>(
        foundation::LogSeverity::kDebug,
        "Shutting down the application"
        );

      OnShutdown();

      ShutdownServices();

      instance_ = nullptr;
    }

    //--------------------------------------------------------------------------
    void Application::OnInitialize()
    {

    }

    //--------------------------------------------------------------------------
    void Application::OnUpdate(float dt)
    {

    }

    //--------------------------------------------------------------------------
    void Application::OnFixedUpdate(float time_step)
    {

    }

    //--------------------------------------------------------------------------
    void Application::OnRender(float dt)
    {

    }

    //--------------------------------------------------------------------------
    void Application::OnShutdown()
    {

    }

    //--------------------------------------------------------------------------
    void Application::InitializeServices()
    {
      for (size_t i = 0; i < services_.size(); ++i)
      {
        services_.at(i)->OnInitialize(*this);
      }
    }

    //--------------------------------------------------------------------------
    void Application::ShutdownServices()
    {
      for (size_t i = 0; i < services_.size(); ++i)
      {
        services_.at(i)->OnShutdown(*this);
      }

      services_.clear();
    }
  }
}