#pragma once

#define SCRIPT_CLASS(...)
#define SCRIPT_FUNC(...)
#define SCRIPT_FUNC_EX(...)
#define SCRIPT_EXPAND(...)
#define SCRIPT_ENUM(...)

#ifndef SNUFF_NSCRIPTING
#define SCRIPT_CONSTRUCTOR(x) x(snuffbox::scripting::ScriptArgs&)
#define SCRIPT_NAME(x)                                                        \
static const char* ScriptName() { return #x; }                                \
static void RegisterScriptFunctions(snuffbox::scripting::ScriptRegister*);    \
static void RegisterScriptEnums(snuffbox::scripting::ScriptRegister*)                  
#else
#define SCRIPT_CONSTRUCTOR(x)
#define SCRIPT_NAME(x)
#endif

#ifdef SNUFF_DUKTAPE
#include "duk/duk_register.h"
namespace snuffbox
{
  namespace scripting
  {
    using ScriptRegister = DukRegister;
  }
}
#endif

namespace snuffbox
{
  namespace scripting
  {
    /**
    * @brief The base class for every class to be exposed to scripting to use
    *
    * The static functions that should be exposed are:
    *
    * static const char* ScriptName()
    * static void RegisterScriptFunctions(ScriptRegister*)
    * static void RegisterScriptEnums(ScriptRegister*)
    *
    * If constructable; ClassName(ScriptArgs&)
    *
    * These functions can be declared through the SCRIPT_NAME macro.
    *
    * The functions will be compiled away when using SNUFF_NSCRIPTING, so that
    * no scripting code is included in C++-only builds. The functions are then
    * to be implemented by including the source defintions created by the
    * script parser utility.
    *
    * @remarks This class doesn't really implement any functionality, as the
    *          "derived" interface is purely static. By deriving from this class
    *          we just guarantee an exposed class was actually intended to be
    *          scriptable.
    *
    * @author Daniel Konings
    */
    class ScriptClass
    {
      
    };
  }
}