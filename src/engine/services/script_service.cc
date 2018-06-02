#include "engine/services/script_service.h"

#include "engine/auxiliary/debug.h"
#include "engine/definitions/keycodes.h"
#include "engine/definitions/components.h"
#include "engine/services/input_service.h"
#include "engine/ecs/entity.h"
#include "engine/components/transform_component.h"

#ifndef SNUFF_NSCRIPTING
#include "engine/components/script_component.h"
#include <sparsed/keycodes.gen.cc>
#include <sparsed/components.gen.cc>
#endif

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    ScriptService::ScriptService() :
      ServiceBase<ScriptService>("ScriptService")
    {
      state_ = foundation::Memory::ConstructUnique<scripting::ScriptState>(
        &foundation::Memory::default_allocator());
    }

    //--------------------------------------------------------------------------
    bool ScriptService::CompileAndRun(
      const char* ctx, 
      const char* src,
      bool print)
    {
      if (state_ == nullptr)
      {
        return false;
      }

      bool success = state_->CompileFromSource(ctx, src, print);

      InitializeCallbacks();

      return success;
    }

    //--------------------------------------------------------------------------
    void ScriptService::OnStartCallback()
    {
      on_start_.Call();
    }

    //--------------------------------------------------------------------------
    void ScriptService::OnUpdateCallback(float dt)
    {
      on_update_.Call(dt);
    }

    //--------------------------------------------------------------------------
    void ScriptService::OnFixedUpdateCallback(float time_step)
    {
      on_fixed_update_.Call(time_step);
    }

    //--------------------------------------------------------------------------
    void ScriptService::OnRenderCallback(float dt)
    {
      on_render_.Call(dt);
    }

    //--------------------------------------------------------------------------
    void ScriptService::OnShutdownCallback()
    {
      on_shutdown_.Call();
    }

    //--------------------------------------------------------------------------
    void ScriptService::OnReloadCallback(const foundation::String& path)
    {
      on_reload_.Call(path);
    }

    //--------------------------------------------------------------------------
    scripting::ScriptState* ScriptService::state() const
    {
      return state_.get();
    }

    //--------------------------------------------------------------------------
    foundation::ErrorCodes ScriptService::OnInitialize(Application& app)
    {
      return Initialize() == true ? 
        foundation::ErrorCodes::kSuccess : 
        foundation::ErrorCodes::kScriptInitializationFailed;
    }

    //--------------------------------------------------------------------------
    void ScriptService::OnShutdown(Application& app)
    {
      if (state_ != nullptr)
      {
        state_->Shutdown();
      }
    }

    //--------------------------------------------------------------------------
    bool ScriptService::Initialize()
    {
      if (state_->Initialize() == false)
      {
        return false;
      }

      register_ = 
        foundation::Memory::ConstructUnique<scripting::ScriptRegister>(
          &foundation::Memory::default_allocator(),
          state_.get());

      RegisterClasses();

      return true;
    }

    //--------------------------------------------------------------------------
    void ScriptService::RegisterClasses()
    {
      register_->RegisterClass<Debug>();
      register_->RegisterClass<InputService>();
      register_->RegisterClass<Entity, true>();
      register_->RegisterClass<TransformComponent>();
      register_->RegisterClass<ScriptComponent>();

      register_->RegisterEnum<Keys>();
      register_->RegisterEnum<MouseButtons>();
      register_->RegisterEnum<JoystickButtons>();
      register_->RegisterEnum<JoystickAxes>();
      register_->RegisterEnum<Components>();
    }

    //--------------------------------------------------------------------------
    void ScriptService::InitializeCallbacks()
    {
      scripting::ScriptCallback<>::FromGlobal(
        state_.get(), 
        "OnStart", 
        &on_start_);

      scripting::ScriptCallback<float>::FromGlobal(
        state_.get(), 
        "OnUpdate", 
        &on_update_);

      scripting::ScriptCallback<float>::FromGlobal(
        state_.get(), 
        "OnFixedUpdate", 
        &on_fixed_update_);

      scripting::ScriptCallback<float>::FromGlobal(
        state_.get(), 
        "OnRender", 
        &on_render_);

      scripting::ScriptCallback<>::FromGlobal(
        state_.get(), 
        "OnShutdown", 
        &on_shutdown_);
    }
  }
}