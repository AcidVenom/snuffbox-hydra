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
      id_(0),
      state_(nullptr)
    {
      id_ = kCurrentID_;
      ++kCurrentID_;
    }

    //--------------------------------------------------------------------------
    size_t ScriptClass::id() const
    {
      return id_;
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