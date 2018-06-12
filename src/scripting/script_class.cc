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
      is_from_script_(false),
      script_id_(0),
      state_(nullptr)
    {
      script_id_ = kCurrentID_;
      ++kCurrentID_;
    }

    //--------------------------------------------------------------------------
    size_t ScriptClass::script_id() const
    {
      return script_id_;
    }

    //--------------------------------------------------------------------------
    void ScriptClass::set_state(ScriptState* state)
    {
      state_ = state;
    }

    //--------------------------------------------------------------------------
    ScriptClass::~ScriptClass()
    {
      if (state_ != nullptr && is_from_script_ == false)
      {
        state_->FinalizeClass(this);
      }
    }
  }
}