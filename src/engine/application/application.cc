#include "engine/application/application.h"

#include "engine/auxiliary/debug.h"

#include "engine/cvar/command_line_parser.h"

#include "engine/services/cvar_service.h"
#include "engine/services/window_service.h"

#ifndef SNUFF_NSCRIPTING
#include "engine/services/script_service.h"
#include <scripting/scripting.h>
#endif

#include <thread>
#include <chrono>
#include <cassert>
#include <iostream>

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
      1,
      1280,
      720
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
        \tWindow width: {3}\n\
        \tWindow height: {4}\n\
        ",
        config_.application_name,
        config_.version_string,
        config_.verbosity,
        config_.window_width,
        config_.window_height);
    }

    //--------------------------------------------------------------------------
    foundation::ErrorCodes Application::Run()
    {
      ApplyConfiguration();

      Debug::LogVerbosity<2>(
        foundation::LogSeverity::kDebug,
        "Initializing the application"
        );

      foundation::ErrorCodes err = Initialize();

      if (err != foundation::ErrorCodes::kSuccess)
      {
        Shutdown();
        return err;
      }

      WindowService* window = GetService<WindowService>();

      while (window->ProcessEvents() == false)
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
      }

      Shutdown();

      return foundation::ErrorCodes::kSuccess;
    }

    //--------------------------------------------------------------------------
    const Application::Configuration& Application::config() const
    {
      return config_;
    }

    //--------------------------------------------------------------------------
    foundation::ErrorCodes Application::Initialize()
    {
      CVarService* cvar = CreateService<CVarService>();

      CreateService<WindowService>();

#ifndef SNUFF_NSCRIPTING
      CreateService<ScriptService>();
#endif

      foundation::ErrorCodes err = InitializeServices();
      if (err != foundation::ErrorCodes::kSuccess)
      {
        return err;
      }

      OnInitialize();
      cvar->RegisterFromCLI(cli_);

      return foundation::ErrorCodes::kSuccess;
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
    foundation::ErrorCodes Application::InitializeServices()
    {
      foundation::ErrorCodes err;

      for (size_t i = 0; i < services_.size(); ++i)
      {
        if ((err = services_.at(i)->OnInitialize(*this)) 
          != foundation::ErrorCodes::kSuccess)
        {
          Debug::LogVerbosity<1>(
            foundation::LogSeverity::kFatal,
            "Could not initialize service '{0}'",
            services_.at(i)->name()
            );

          return err;
        }
      }

      return foundation::ErrorCodes::kSuccess;
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
