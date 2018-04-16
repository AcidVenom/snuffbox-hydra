#include "engine/services/script_service.h"

#include "engine/auxiliary/debug.h"
#include "engine/definitions/keycodes.h"

#include <sparsed/keycodes.gen.cc>

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
    bool ScriptService::CompileAndRun(const char* src)
    {
      if (state_ == nullptr)
      {
        return false;
      }

      return state_->CompileFromSource("application", src, true);
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
      InitializeCallbacks();

      return true;
    }

    //--------------------------------------------------------------------------
    void ScriptService::RegisterClasses()
    {
      register_->RegisterClass<Debug>();

      register_->RegisterEnum<Keys>();
      register_->RegisterEnum<MouseButtons>();
    }

    //--------------------------------------------------------------------------
    void ScriptService::InitializeCallbacks()
    {
      
    }
  }
}