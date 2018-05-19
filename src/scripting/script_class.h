#pragma once

#define SCRIPT_CLASS(...)
#define SCRIPT_FUNC(...)
#define SCRIPT_FUNC_EX(...)
#define SCRIPT_EXPAND(...)
#define SCRIPT_ENUM(...)

#ifndef SNUFF_NSCRIPTING

#define SCRIPT_CONSTRUCTOR(x) x(snuffbox::scripting::ScriptArgs&)

#define SCRIPT_NAME(x)                                                        \
static const char* ScriptName() { static const char* name = #x; return name; }\
static void RegisterScriptFunctions(snuffbox::scripting::ScriptRegister*);

#define SCRIPT_ENUM_DECL(x)                                                   \
namespace snuffbox                                                            \
{                                                                             \
  namespace scripting                                                         \
  {                                                                           \
    template <>                                                               \
    void ScriptClass::RegisterScriptEnum<x>(                                  \
      snuffbox::scripting::ScriptRegister*);                                  \
  }                                                                           \
}

#define SPARSE_CUSTOM(cl, func) bool sparse_##cl##_##func                     \
(snuffbox::scripting::ScriptArgs& args)

#include "scripting/script_args.h"
#else
#define SCRIPT_CONSTRUCTOR(x)
#define SCRIPT_NAME(x)
#define SCRIPT_ENUM_DECL(x)
#define SCRIPT_CONSTRUCTOR(x)
#endif

#ifdef SNUFF_DUKTAPE
namespace snuffbox
{
  namespace scripting
  {
    class DukRegister;
    using ScriptRegister = DukRegister;

    class DukState;
    using ScriptState = DukState;
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
#ifndef SNUFF_NSCRIPTING

    public:

      /**
      * @brief Default constructor, assigns a script ID
      */
      ScriptClass();

      /**
      * @brief This function should be specialized with an enumerator
      *        as T parameter, to register enums using ScriptEnumRegister
      *
      * @tparam T The enumerator to register
      *
      * @param[in] reg The register to register the enumerator in
      */
      template <typename T>
      static void RegisterScriptEnum(ScriptRegister* reg);

      /**
      * @brief Sets the script state
      *
      * @param[in] state The state to set
      */
      void set_script_state(ScriptState* state);

      /**
      * @return The script ID of this class
      */
      size_t id() const;

      /**
      * @brief Removes the script ID reference
      */
      virtual ~ScriptClass();

    private:

      size_t id_; //!< The script ID of this class
      ScriptState* state_; //!< The current script state

      static size_t kCurrentID_; //!< The current ID to assign
#endif
    };
  }
}
