#pragma once

#include "engine/cvar/cvar.h"
#include "engine/definitions/cli.h"
#include "engine/services/service.h"

#include <foundation/memory/memory.h>
#include <foundation/containers/map.h>

namespace snuffbox
{
  namespace engine
  {
    /**
    * @brief Used to manage CVars both from CLI and that were registered
    *        manually in code.
    *
    * @author Daniel Konings
    */
    class CVarService : public ServiceBase<CVarService>
    {

    public:

      /**
      * @brief Default constructor
      */
      CVarService();

    protected:

      /**
      * @brief Registers a CVar to the list of registered CVars in this service
      *
      * @tparam T The type of CVar to register
      *
      * @param[in] name The name to register the CVar under
      * @param[in] description The description of the CVar
      * @param[in] def The default value if not set from CLI
      *
      * @return The pointer to the registered CVar
      */
      template <typename T>
      CVarValue* RegisterImpl(
        const char* name, 
        const char* description, 
        const T& def);

    public:

      /**
      * @brief Registers values that haven't been registered yet from the CLI
      *
      * The parsed command line will automatically register values that aren't
      * registered yet, along with setting values that are set through CLI.
      *
      * @param[in] cli The parsed command line from the application
      */
      void RegisterFromCLI(const CLI& cli);

      /**
      * @brief Executes a parsed command of the CommandLineParser
      *
      * @param[in] cmd The command to execute
      */
      void ExecuteCommand(const CLICommand& cmd);

      /**
      * @brief Logs a value of a CVar
      *
      * @param[in] value The CVar to log the value of
      */
      void LogValue(const CVarValue* value) const;

      /**
      * @see CVarService::LogValue
      *
      * @remarks This will log <undefined> if the CVar is not registered
      *
      * @param[in] name The name of the CVar to log the value of
      */
      void LogValue(const foundation::String& name) const;

      /**
      * @brief Logs the description of a CVar
      *
      * @param[in] value The CVar to log the description of
      */
      void LogDescription(const CVarValue* value) const;

      /**
      * @see CVarService::LogDescription
      *
      * @remarks This will log <undefined> if the CVar is not registered
      *
      * @param[in] name The name of the CVar to log the description of
      */
      void LogDescription(const foundation::String& name) const;

      /**
      * @brief Log all components of each CVar that is currently registered
      */
      void LogAll() const;

      /**
      * @brief The registry implementation for non-double CVar values
      *
      * @see CVarService::RegisterImpl
      */
      template <typename T, typename = CVarValue::is_double<T, false>>
      CVarValue* Register(
        const char* name, 
        const char* description, 
        const T& def)
      {
        return RegisterImpl(name, description, def);
      }

      /**
      * @brief The registry implementation for double CVar values
      *
      * @see CVarService::RegisterImpl
      *
      * @param[in] range The valid range to set for this numerical CVar
      */
      template <typename T, typename = CVarValue::is_double<T, true>>
      CVarValue* Register(
        const char* name,
        const char* description,
        const T& def,
        const CVarValue::Range& range = CVarValue::Range())
      {
        CVarValue* value = RegisterImpl(name, description, def);
        CVar<double>* dv = static_cast<CVar<double>*>(value);

        dv->SetRange(range);

        return value;
      }

      /**
      * @brief Retrieves a CVar by name
      *
      * The retrieved CVar still needs to be cast to the appropriate derived
      * type. If the CVar does not exist in any context, this function will
      * return nullptr.
      *
      * @param[in] name The name of the CVar to retrieve
      *
      * @return The retrieved CVar, or nullptr if it doesn't exist
      */
      CVarValue* Get(const foundation::String& name) const;

      /**
      * @brief Checks if a CVar value exists
      *
      * @param[in] name The name of the CVar to look for
      *
      * @return Does the value exist?
      */
      bool Exists(const foundation::String& name) const;

    protected:

      /**
      * @see IService::OnInitialize
      */
      bool OnInitialize(Application& app) override;

      /**
      * @see IService::OnShutdown
      */
      void OnShutdown(Application& app) override;

    private:

      static const size_t kLogPadding_; //!< The amount of padding for logging

      /**
      * @brief The CVar map, which consists of a string to value relationship
      */
      using CVarMap = 
        foundation::Map<foundation::String, foundation::UniquePtr<CVarValue>>;

      /**
      * @brief The list of registered CVar values
      */
      CVarMap registered_;

    };

    //--------------------------------------------------------------------------
    template <typename T>
    inline CVarValue* CVarService::RegisterImpl(
      const char* name, 
      const char* description, 
      const T& def)
    {
      foundation::UniquePtr<CVarValue> value =
        foundation::Memory::ConstructUnique<CVar<T>>(
          &foundation::Memory::default_allocator(),
          name,
          description,
          def);

      CVarValue* ptr = value.get();

      registered_.emplace(
        eastl::pair<foundation::String, foundation::UniquePtr<CVarValue>>
        { 
          name, eastl::move(value) 
        });

      return ptr;
    }
  }
}