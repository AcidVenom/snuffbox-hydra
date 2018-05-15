#pragma once

#include "engine/services/service.h"
#include "engine/definitions/cli.h"
#include "engine/auxiliary/debug.h"

#include <foundation/definitions/error_codes.h>
#include <foundation/memory/memory.h>
#include <foundation/containers/vector.h>

#include <cinttypes>

namespace snuffbox
{
  namespace engine
  {
    class WindowService;

    /**
    * @brief The application class to use as entry point for Snuffbox
    *
    * This class can be inherited from to implement the functions that are
    * executed during certain events. These functions include the following:
    *
    * - Application::OnInitialize
    * - Application::OnUpdate
    * - Application::OnFixedUpdate
    * - Application::OnRender
    * - Application::OnShutdown
    *
    * @remarks The application should release all its allocated memory before
    *          the destructor is called, to guarantee that leaks are checked
    *          properly and no memory is floating around.
    *
    * @author Daniel Konings
    */
    class Application
    {

    public:

      /**
      * @brief Used to set different properties for application execution
      *
      * @author Daniel Konings
      */
      struct Configuration
      {
        const char* application_name; //!< The name of the application
        const char* version_string; //!< The version string

        bool editor_mode; //!< Are we in editor mode?

        /**
        * @brief The verbosity level of the application
        *
        * This determines which logs will be output throughout the execution
        * of the application.
        */
        uint32_t verbosity;

        uint16_t window_width; //!< The window width
        uint16_t window_height; //!< The window height

        /**
        * @brief The default configuration
        */
        static Configuration kDefaultConfiguration;
      };

      /**
      * @brief Construct by providing the command line arguments
      *
      * The command line arguments are automatically parsed into the CVar
      * interface for use.
      *
      * Supported types are:
      * - Booleans
      * - Strings
      * - Numbers
      *
      * @param[in] argc The number of arguments passed
      * @param[in] argv The actual argument list
      * @param[in] config The configuration for the application to use
      */
      Application(
        int argc = 0, 
        char** argv = nullptr,
        const Configuration& config = Configuration::kDefaultConfiguration);

      /**
      * @brief Executes the application and starts the main loop
      *
      * @return The error code, succesful with ErrorCodes::kSuccess
      */
      virtual foundation::ErrorCodes Run();

      /**
      * @brief Retrieves a service in the application
      *
      * @tparam T The service to retrieve
      *
      * @return The service, or nullptr if it was never created or assigned
      *         a type ID
      */
      template <typename T>
      T* GetService();
      
      /**
      * @brief Notify the application that it should quit
      */
      void NotifyQuit();

      /**
      * @return The instance of the application
      */
      static Application* Instance();

      /**
      * @return The configuration of the application
      */
      const Configuration& config() const;

      /**
      * @return Should the application quit?
      */
      bool should_quit() const;

    protected:

      /**
      * @brief Applies the current configuration from Application::config_
      */
      void ApplyConfiguration();

      /**
      * @brief Initializes the application
      *
      * The initialization order is as follows:
      *
      * 1) Application::CreateServices
      * 2) Application::InitializeServices
      * 3) Application::RegisterCVars
      * 4) Application::OnInitialize
      * 5) Optionally call the script's OnInitialize
      *
      * @return The error code, or ErrorCodes::kSuccess if everything
      *         was initialized succesfully
      */
      foundation::ErrorCodes Initialize();

      /**
      * @brief Create all required services
      */
      void CreateServices();

      /**
      * @brief Initializes all services
      *
      * After initialization of the main services, the renderer service is
      * created and initialized as well
      *
      * @see Application::CreateRenderer
      * @see Application::InitializeRenderer
      *
      * @return The error code, or ErrorCodes::kSuccess if every service
      *         was initialized succesfully
      */
      foundation::ErrorCodes InitializeServices();

      /**
      * @brief Shuts down all services
      *
      * @remarks All memory should be deallocated appropriately here
      */
      void ShutdownServices();

      /**
      * @brief Creates the renderer service
      *
      * This method should be overridden by the editor application to
      * use a different graphics window over the GLFW window used in the
      * regular player application.
      */
      virtual void CreateRenderer();

      /**
      * @brief Initializes the renderer after initializing all other services
      *
      * @return The error code, or ErrorCodes::kSuccess if initialization was
      *         a success
      */
      foundation::ErrorCodes InitializeRenderer();

      /**
      * @brief Registers all CVars from the different systems and set
      *        the CVars from the command-line
      */
      void RegisterCVars();

      /**
      * @brief Calls the update functions, to update data real-time
      *
      * @param[in] dt The current delta-time of the application
      *
      * Calls Application::OnUpdate
      */
      void Update(float dt);

      /**
      * @brief Calls the fixed update functions, to update data in a fixed
      *        interval/with fixed iterations.
      *
      * @param[in] time_step The fixed time step
      *
      * Calls Application::OnFixedUpdate
      */
      void FixedUpdate(float time_step);

      /**
      * @brief Renders the data within the renderer of this current frame
      *
      * @param[in] dt The current delta-time of the application
      *
      * Calls Application::OnRender
      */
      void Render(float dt);

      /**
      * @brief Called when the application is shut down, before the destructor
      *
      * Calls Application::OnShutdown
      *
      * @remarks Memory leaks will not be checked here, but after the
      *          the main entry point execution has returned. Make sure all
      *          internal memory is properly deallocated before that.
      */
      void Shutdown();

      /**
      * @brief The initialize function to override if you want custom behavior
      *        in a derived version of the application
      *
      * @remarks The base implementation does nothing
      *
      * @see Application::Initialize
      */
      virtual void OnInitialize();

      /**
      * @brief The update function to override if you want custom behavior
      *        in a derived version of the application
      *
      * @remarks The base implementation does nothing
      *
      * @see Application::Update
      */
      virtual void OnUpdate(float dt);

      /**
      * @brief The fixed update function to override if you want custom behavior
      *        in a derived version of the application
      *
      * @remarks The base implementation does nothing
      *
      * @see Application::FixedUpdate
      */
      virtual void OnFixedUpdate(float time_step);

      /**
      * @brief The render function to override if you want custom behavior
      *        in a derived version of the application
      *
      * @remarks The base implementation does nothing
      *
      * @see Application::Render
      */
      virtual void OnRender(float dt);

      /**
      * @brief The shutdown function to override if you want custom behavior
      *        in a derived version of the application
      *
      * @remarks The base implementation does nothing
      *
      * @see Application::Shutdown
      */
      virtual void OnShutdown();

      /**
      * @brief Creates a service contained by the application
      *
      * @tparam T A service derived from IService
      * @tparam Args... The arguments to pass into the constructor of the
      *                 service
      *
      * @param[in] args The arguments to pass
      *
      * @return The created service
      */
      template <typename T, typename ... Args>
      T* CreateService(Args&&... args);

    private:

      /**
      * @brief A vector of services to contain all services in by type ID
      */
      using Services = foundation::Vector<foundation::UniquePtr<IService>>;

      Configuration config_; //!< The configuration of the application
      CLI cli_; //!< The parsed command line

      Services services_; //!< The list of services that are available

      bool should_quit_; //!< Should the application quit?

      static Application* instance_; //!< The current application instance
    };

    //--------------------------------------------------------------------------
    template <typename T>
    inline T* Application::GetService()
    {
      static_assert(eastl::is_base_of<IService, T>::value,
        "Only services that derive from IService \
        can be retrieved as a service from the application");

      uint32_t id = T::type_id();
      if (id >= static_cast<uint32_t>(services_.size()))
      {
        Debug::LogVerbosity<1>(
          foundation::LogSeverity::kFatal,
          "Requested an invalid service"
          );
        return nullptr;
      }

      return static_cast<T*>(services_.at(id).get());
    }

    //--------------------------------------------------------------------------
    template <typename T, typename ... Args>
    inline T* Application::CreateService(Args&&... args)
    {
      static_assert(eastl::is_base_of<IService, T>::value, 
        "Only services that derive from IService \
        can be added as a service to the application");

      foundation::UniquePtr<T> ptr = 
        foundation::Memory::ConstructUnique<T>(
          &foundation::Memory::default_allocator(),
          eastl::forward<Args>(args)...);

      T* pptr = ptr.get();

      services_.push_back(eastl::move(ptr));

      return pptr;
    }
  }
}
