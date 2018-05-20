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
      id_(0)
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
    ScriptClass::~ScriptClass()
    {
      
    }
  }
}