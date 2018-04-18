#pragma once

#include "scripting/script_type_traits.h"
#include "scripting/script_value.h"
#include "scripting/script_args.h"

#include "scripting/duk/duk_definitions.h"

#include <foundation/containers/vector.h>

#include <duktape.h>

#include <string>

namespace snuffbox
{
  namespace scripting
  {
    /**
    * @brief Used to wrap the stack of duktape and automatically do value
    *        retrieving and casting without having to check types
    *
    * @author Daniel Konings
    */
    class DukWrapper
    {

    public:

      /**
      * @brief Construct through a duktape context
      *
      * @param[in] ctx The context to use for all function calls
      */
      DukWrapper(duk_context* ctx);

      /**
      * @brief Used to push numerical and enumerator values
      *
      * @see DukWrapper::PushValueImpl
      */
      template <typename T>
      void PushValue(
        T value, 
        if_number_or_enum<T>* = nullptr) const;

      /**
      * @brief Used to push any value that is both not an enumeration,
      *        nor a numerical value
      *
      * @see DukWrapper::PushValueImpl
      */
      template <typename T>
      void PushValue(
        T value,
        if_n_number_and_enum<T>* = nullptr) const;

      /**
      * @brief Pushes a pointer by type, by retrieving its meta information
      *        from the global object
      *
      * If the callee's pointer is the same as the pointer requested,
      * the object is simply duplicated. This keeps reference counting intact
      * and makes sure there are no double deletions of objects.
      *
      * @param[in] callee The callee
      * @param[in] ptr The pointer to push
      * @param[in] type The type name of the pointer
      */
      void PushPointer(void* callee, void* ptr, const char* type) const;

    protected:

      /**
      * @brief Pushes a value onto the stack
      *
      * @tparam T The type of the value to push
      *
      * The supported types are basically the values that derive from 
      * ScriptValue and that are usually accessible in scripting
      *
      * @param[in] value The value to push
      */
      template <typename T>
      void PushValueImpl(T value) const;

    public:

      /**
      * @brief Retrieves a value from the duktape stack as a ScriptHandle
      *
      * @param[in] stack_idx The index of the value to retrieve
      */
      ScriptHandle GetValueAt(duk_idx_t stack_idx) const;

      /**
      * @brief Retieves an object from the duktape stack as a ScriptObject
      *
      * If the object is indexed by contiguous indices, the object is treated
      * as an array.
      *
      * @see DukWrapper::GetValueAt
      */
      ScriptHandle GetObjectAt(duk_idx_t stack_idx) const;


      /**
      * @brief Retrieves the current arguments from a duktape context
      *
      * @tparam T The callee's type, used for member functions
      *
      * If the callee's type doesn't match the DUK_HIDDEN_NAME property and
      * the function is a member function, the callee will be set to nullptr
      *
      * @param[out] args The converted arguments
      */
      template <typename T>
      void GetArguments(ScriptArgs* args) const;

    private:

      duk_context* context_; //!< The context for this wrapper
    };

    //--------------------------------------------------------------------------
    template <typename T>
    inline void DukWrapper::PushValue(T value, if_number_or_enum<T>*) const
    {
      PushValueImpl<double>(static_cast<double>(value));
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void DukWrapper::PushValue(T value, if_n_number_and_enum<T>*) const
    {
      PushValueImpl<T>(value);
    }

    //--------------------------------------------------------------------------
    template <typename T>
    void DukWrapper::GetArguments(ScriptArgs* args) const
    {
      if (args == nullptr)
      {
        return;
      }

      duk_context* ctx = context_;

      void* callee = nullptr;

      duk_push_this(ctx);

      if (duk_get_prop_string(ctx, -1, DUK_HIDDEN_NAME) != 0)
      {
        bool is_same = strcmp(duk_get_string(ctx, -1), T::ScriptName()) == 0;

        if (is_same && duk_get_prop_string(ctx, -2, DUK_HIDDEN_PTR) != 0)
        {
          callee = duk_get_pointer(ctx, -1);
          duk_pop(ctx);
        }
      }

      duk_pop_2(ctx);

      duk_idx_t argc = duk_get_top(ctx);

      foundation::Vector<ScriptHandle> argv;
      argv.resize(argc);

      for (duk_idx_t i = 0; i < argc; ++i)
      {
        argv.at(i) = GetValueAt(i);
      }

      *args = ScriptArgs(argv, callee);
    }
  }
}
