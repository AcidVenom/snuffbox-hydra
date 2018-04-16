#include "engine/services/script_service.h"
#include "engine/auxiliary/debug.h"

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
    bool ScriptService::OnInitialize(Application& app)
    {
      return Initialize();
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

      RegisterClasses();
      InitializeCallbacks();

      return true;
    }

    //--------------------------------------------------------------------------
    void ScriptService::RegisterClasses()
    {
      register_ = 
        foundation::Memory::ConstructUnique<scripting::ScriptRegister>(
          &foundation::Memory::default_allocator(),
          state_.get());

      register_->RegisterClass<Debug>();
    }

    //--------------------------------------------------------------------------
    void ScriptService::InitializeCallbacks()
    {
      
    }
  }
}