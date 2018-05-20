#pragma once

#include "engine/services/service.h"

#include <scripting/scripting.h>
#include <foundation/memory/memory.h>

namespace snuffbox
{
  namespace engine
  {
    /**
    * @brief Used to interface the scripting functionalities of
    *        snuffbox-scripting
    *
    * This class is unused if the scripting component of
    * Snuffbox is disabled (SNUFF_NSCRIPTING)
    *
    * @author Daniel Konings
    */
    class ScriptService : public ServiceBase<ScriptService>
    {

    public:

      /**
      * @brief Default constructor, creates the scripting objects
      */
      ScriptService();

      /**
      * @brief Runs script code in the script state
      *
      * @param[in] ctx The context to run the code in
      * @param[in] src The code to compile and run
      * @param[in] print Should the result be printed to the console?
      *
      * @remarks Every time a script is compiled, the callbacks are
      *          reinitialized for reloading
      *
      * @see ScriptService::InitializeCallbacks
      *
      * @return Was running the code a success?
      */
      bool CompileAndRun(const char* ctx, const char* src, bool print = false);

      /**
      * @see ScriptService::InitializeCallbacks
      */
      void OnInitializeCallback();

      /**
      * @see ScriptService::InitializeCallbacks
      */
      void OnUpdateCallback(float dt);

      /**
      * @see ScriptService::InitializeCallbacks
      */
      void OnFixedUpdateCallback(float time_step);

      /**
      * @see ScriptService::InitializeCallbacks
      */
      void OnRenderCallback(float dt);

      /**
      * @see ScriptService::InitializeCallbacks
      */
      void OnShutdownCallback();

      /**
      * @see ScriptService::InitializeCallbacks
      */
      void OnReloadCallback(const foundation::String& path);

      /**
      * @return The current scripting state
      */
      scripting::ScriptState* state() const;

    protected:

      /**
      * @see IService::OnInitialize
      */
      foundation::ErrorCodes OnInitialize(Application& app) override;

      /**
      * @see IService::OnShutdown
      */
      void OnShutdown(Application& app) override;

      /**
      * @brief Initializes the script state, the script register and binds all
      *        callbacks
      *
      * @return Was initialization a success?
      */
      bool Initialize();

      /**
      * @brief Registers all classes to be exposed to scripting using
      *        ScriptRegister
      *
      * @see ScriptService::register_
      */
      void RegisterClasses();

      /**
      * @brief Initializes all the callbacks of the main script file
      *
      * The callbacks to be set are:
      *
      * - OnInitialize, called when the application is initialized
      * - OnUpdate, called in the variable update
      * - OnFixedUpdate, called in the fixed update
      * - OnRender, called before a frame is rendered
      * - OnShutdown, called when the application is shut down
      * - OnReload, called when a script file is reloaded
      *
      */
      void InitializeCallbacks();

    private:

      /**
      * @brief The script state 
      */
      foundation::UniquePtr<scripting::ScriptState> state_;

      /**
      * @brief The script register to register classes and enums with
      */
      foundation::UniquePtr<scripting::ScriptRegister> register_;

      /**
      * @see ScriptService::InitializeCallbacks
      */
      scripting::ScriptCallback<> on_initialize_;

      /**
      * @see ScriptService::InitializeCallbacks
      *
      * @remarks The parameter passed is the current delta-time
      */
      scripting::ScriptCallback<float> on_update_;

      /**
      * @see ScriptService::InitializeCallbacks
      *
      * @remarks The parameter passed is the current fixed time step
      */
      scripting::ScriptCallback<float> on_fixed_update_;

      /**
      * @see ScriptService::InitializeCallbacks
      *
      * @remarks The parameter passed is the current delta-time
      */
      scripting::ScriptCallback<float> on_render_;

      /**
      * @see ScriptService::InitializeCallbacks
      */
      scripting::ScriptCallback<> on_shutdown_;

      /**
      * @see ScriptService::InitializeCallbacks
      *
      * @remarks The parameter passed is the path to the file that was reloaded
      */
      scripting::ScriptCallback<foundation::String> on_reload_;
    };
  }
}