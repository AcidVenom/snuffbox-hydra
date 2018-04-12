#pragma once

#include "scripting/script_register.h"
#include "scripting/duk/duk_definitions.h"
#include "scripting/duk/duk_wrapper.h"

#include <foundation/memory/memory.h>

#include <duktape.h>

namespace snuffbox
{
  namespace scripting
  {
    class DukState;
    class ScriptClass;

    /**
    * @brief Used to register C++ classes to the scripting environment for use
    *
    * @author Daniel Konings
    */
    class DukRegister
    {

    public:

      /**
      * @brief Construct by providing the current DukState
      *
      * @param[in] state The current state
      */
      DukRegister(DukState* state);

      /**
      * @brief Registers a class
      *
      * The class to be registered needs to derive from ScriptClass. This
      * is not 100% necessary, but it includes some form of safety checking.
      *
      * @see ScriptClass
      *
      * @tparam T The class to register
      * @tparam C Is the class constructable?
      */
      template <typename T, bool C = false>
      void RegisterClass();

      /**
      * @brief Registers a ScriptFunctionRegister list into the script state
      *
      * @remarks This register will continue until "nullptr" is found for the
      *          next function name or function pointer
      *
      * @param[in] reg The function register to register
      */
      void RegisterFunctions(ScriptFunctionRegister* reg);

      /**
      * @brief Registers a ScriptEnum list into the script state
      *
      * @remarks This register will continue until "nullptr" is found for the
      *          next enumerator name
      *
      * @param[in] reg The enum register to register
      */
      void RegisterEnums(ScriptEnum* reg);

    protected:

      /**
      * @brief Used to specialize behavior for whether a class is constructable
      *        or not.
      *
      * When a class is constructable, its 'prototype' is filled in with
      * the corresponding functions that belong to that class. If not, a new
      * object is simply created and filled in with the functions.
      *
      * @author Daniel Konings
      */
      template <bool C>
      struct Constructable;

      /**
      * @brief The duktape C-function to use as a constructor
      *
      * This function constructs the hidden pointer in the object
      *
      * @tparam T The type to construct C++ sided
      *
      * @remarks Any constructable class should have the MyClass(ScriptArgs&)
      *          constructor defined
      */
      template <typename T>
      static duk_ret_t New(duk_context* ctx);

      /**
      * @brief The duktape C-function to use as a finalizer
      *
      * This function deletes the pointer that was underlying in the object
      *
      * @tparam T The type to destruct after garbage collection
      */
      template <typename T>
      static duk_ret_t Delete(duk_context* ctx);

    private:

      duk_context* context_; //!< The context this register is operating on
    };

    //--------------------------------------------------------------------------
    template <>
    struct DukRegister::Constructable<true>
    {
      template <typename T>
      static void StartClass(duk_context* ctx)
      {
        duk_push_c_function(ctx, &DukRegister::New<T>, DUK_VARARGS);
        duk_push_object(ctx);
      }

      template <typename T>
      static void EndClass(duk_context* ctx)
      {
        duk_put_prop_string(ctx, -2, "prototype");
        duk_put_global_string(ctx, T::ScriptName());
      }
    };

    //--------------------------------------------------------------------------
    template <>
    struct DukRegister::Constructable<false>
    {
      template <typename T>
      static void StartClass(duk_context* ctx)
      {
        duk_push_object(ctx);
      }

      template <typename T>
      static void EndClass(duk_context* ctx)
      {
        duk_put_global_string(ctx, T::ScriptName());
      }
    };

    //--------------------------------------------------------------------------
    template <typename T, bool C>
    inline void DukRegister::RegisterClass()
    {
      static_assert(eastl::is_base_of<ScriptClass, T>::value == true,
        "Script class attempted to be registered was not a ScriptObject type");

      duk_context* ctx = context_;

      Constructable<C>::StartClass<T>(ctx);
      T::RegisterScriptFunctions(this);
      Constructable<C>::EndClass<T>(ctx);

      T::RegisterScriptEnums(this);
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline duk_ret_t DukRegister::New(duk_context* ctx)
    {
      if (duk_is_constructor_call(ctx) == 0) 
      {
        return DUK_RET_TYPE_ERROR;
      }

      const char* name = T::ScriptName();

      DukWrapper wrapper = DukWrapper(ctx);

      ScriptArgs args;
      wrapper.GetArguments(&args);

      duk_push_this(ctx);

      duk_dup(ctx, 0);
      duk_put_prop_string(ctx, -2, name);

      T* ptr = foundation::Memory::Construct<T>(
        &foundation::Memory::default_allocator(),
        args
        );

      duk_push_pointer(ctx, ptr);
      duk_put_prop_string(ctx, -2, DUK_HIDDEN_PTR);

      duk_push_c_function(ctx, &DukRegister::Delete<T>, DUK_VARARGS);
      duk_set_finalizer(ctx, -2);

      duk_push_string(ctx, name);
      duk_put_prop_string(ctx, -2, DUK_HIDDEN_NAME);

      auto ToString = [](duk_context* ctx) -> duk_ret_t
      {
        foundation::String str = 
          foundation::String("[Snuffbox ") + T::ScriptName() + "]";

        duk_push_string(ctx, str.c_str());
        return 1;
      };

      duk_push_c_function(ctx, ToString, DUK_VARARGS);
      duk_put_prop_string(ctx, -2, "toString");

      return 0;
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline duk_ret_t DukRegister::Delete(duk_context* ctx)
    {
      duk_idx_t argc = duk_get_top(ctx);
      foundation::Logger::Assert(argc > 0, "duktape finalizer invalid");

      duk_get_prop_string(ctx, 0, DUK_HIDDEN_PTR);
      void* ptr = duk_get_pointer(ctx, -1);

      foundation::Memory::Destruct<T>(reinterpret_cast<T*>(ptr));

      duk_pop(ctx);

      return 0;
    }
  }
}