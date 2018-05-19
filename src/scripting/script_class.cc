#include "scripting/script_class.h"
#include "scripting/scripting.h"

namespace snuffbox
{
  namespace scripting
  {
    //--------------------------------------------------------------------------
    size_t ScriptClass::kCurrentID_ = 0;

    //--------------------------------------------------------------------------
    ScriptClass::ScriptClass() :
      id_(0),
      state_(nullptr)
    {
      id_ = kCurrentID_;
      ++kCurrentID_;
    }

    //--------------------------------------------------------------------------
    void ScriptClass::set_script_state(ScriptState* state)
    {
      state_ = state;
    }

    //--------------------------------------------------------------------------
    size_t ScriptClass::id() const
    {
      return id_;
    }

    //--------------------------------------------------------------------------
    ScriptClass::~ScriptClass()
    {
      if (state_ != nullptr)
      {
        state_->RemoveScriptClass(this);
      }
    }
  }
}