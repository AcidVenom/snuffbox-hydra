#include "engine/application/application.h"

#include "engine/auxiliary/debug.h"

#include "engine/cvar/command_line_parser.h"

#include "engine/services/cvar_service.h"
#include "engine/services/window_service.h"
#include "engine/services/input_service.h"
#include "engine/services/renderer_service.h"

#ifndef SNUFF_NSCRIPTING
#include "engine/services/script_service.h"
#include <scripting/scripting.h>

#define CREATE_SCRIPT_SERVICE() CreateService<ScriptService>();
#define SCRIPT_CALLBACK(x, ...)\
GetService<ScriptService>()->On##x##Callback(__VA_ARGS__)
#else
#define CREATE_SCRIPT_SERVICE()
#define SCRIPT_CALLBACK(x, ...)
#endif

#include <foundation/io/resources.h>

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
      false,
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
      config_(config),
      should_quit_(false)
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
        \tEditor mode: {2}\n\
        \tVerbosity: {3}\n\
        \tWindow width: {4}\n\
        \tWindow height: {5}\n\
        ",
        config_.application_name,
        config_.version_string,
        config_.editor_mode,
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
      RendererService* renderer = GetService<RendererService>();
      window->BindResizeCallback([&](uint16_t width, uint16_t height)
      { 
          renderer->OnResize(width, height);
      });
      window->Show();

      while (
        should_quit_ == false &&
        window->ProcessEvents() == false)
      {
        Update(0.0f);
        renderer->Present();
      }

      Shutdown();


      return foundation::ErrorCodes::kSuccess;
    }

    //--------------------------------------------------------------------------
    void Application::NotifyQuit()
    {
      should_quit_ = true;
    }

    //--------------------------------------------------------------------------
    Application* Application::Instance()
    {
      return instance_;
    }

    //--------------------------------------------------------------------------
    const Application::Configuration& Application::config() const
    {
      return config_;
    }

    //--------------------------------------------------------------------------
    bool Application::should_quit() const
    {
      return should_quit_;
    }

    //--------------------------------------------------------------------------
    foundation::ErrorCodes Application::Initialize()
    {
      CVarService* cvar_service = CreateService<CVarService>();

      WindowService* window_service = nullptr;
      if (config_.editor_mode == false)
      {
        window_service = CreateService<WindowService>();
      }

      InputService* input_service = CreateService<InputService>();

      CREATE_SCRIPT_SERVICE();

      foundation::ErrorCodes err = InitializeServices();
      if (err != foundation::ErrorCodes::kSuccess)
      {
        return err;
      }

      CreateRenderer(window_service);
      err = reinterpret_cast<IService*>(
        GetService<RendererService>())->OnInitialize(*this);

      if (err != foundation::ErrorCodes::kSuccess)
      {
        return err;
      }

      OnInitialize();
      SCRIPT_CALLBACK(Initialize);

      cvar_service->RegisterFromCLI(cli_);

      if (window_service != nullptr)
      {
        input_service->RegisterInputFilter(window_service->GetWindow());
      }

      return foundation::ErrorCodes::kSuccess;
    }

    //--------------------------------------------------------------------------
    void Application::Update(float dt)
    {
      GetService<InputService>()->Flush();

      SCRIPT_CALLBACK(Update, dt);
      OnUpdate(dt);

      std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    //--------------------------------------------------------------------------
    void Application::FixedUpdate(float time_step)
    {
      SCRIPT_CALLBACK(FixedUpdate, time_step);
      OnFixedUpdate(time_step);
    }

    //--------------------------------------------------------------------------
    void Application::Render(float dt)
    {
      SCRIPT_CALLBACK(Render, dt);
      OnRender(dt);
    }

    //--------------------------------------------------------------------------
    void Application::Shutdown()
    {
      Debug::LogVerbosity<2>(
        foundation::LogSeverity::kDebug,
        "Shutting down the application"
        );

      SCRIPT_CALLBACK(Shutdown);
      OnShutdown();

      ShutdownServices();
      foundation::Resources::Shutdown();

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
    void Application::CreateRenderer(WindowService* window)
    {
      if (window == nullptr)
      {
        return;
      }

      CreateService<RendererService>(window->GetGraphicsWindow());
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
      for (int i = static_cast<int>(services_.size()) - 1; i >= 0; --i)
      {
        services_.at(i)->OnShutdown(*this);
      }

      services_.clear();
    }
  }
}
