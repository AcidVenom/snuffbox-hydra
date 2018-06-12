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
    class ScriptClass;
    class DukState;

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
      * @brief Used to push a numerical value
      *
      * @see DukWrapper::PushValueImpl
      */
      template <typename T>
      void PushValue(
        T value, 
        if_number<T>* = nullptr) const;

      /**
      * @brief Used to push an enum value
      *
      * @see DukWrapper::PushValueImpl
      */
      template <typename T>
      void PushValue(
        T value, 
        if_enum<T>* = nullptr) const;

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
      * @param[in] ptr The pointer to push
      * @param[in] type The type name of the pointer
      */
      void PushPointer(ScriptClass* ptr, const char* type) const;

      /**
      * @brief Stashes an object by ID
      *
      * @remarks If the object is constructed C++-sided, a strong reference
      *          is made in the global stash
      *
      * @param[in] id The ID to stash the object at
      * @param[in] from_script Is the object constructed from script?
      * @param[in] stack_idx The stack index of the object
      */
      void StashObject(
        size_t id, 
        bool from_script,
        duk_idx_t stack_idx = -1) const;

      /**
      * @brief Pushes a stashed object onto the stack if it exists
      *
      * @param[in] id The ID to stash the object at
      *
      * @return Was the stashed object found?
      */
      bool PushStashedObject(size_t id) const;

      /**
      * @brief Removes the stashed object for a given script ID
      *
      * @param[in] id The ID to the object to remove
      */
      void RemoveStashedObject(size_t id) const;

      /**
      * @brief Sets the userdata state pointer within the global stash
      *
      * @param[in] state The state to set
      */
      void SetState(DukState* state) const;

      /**
      * @brief Retrieves the userdata state pointer from the global stash
      *
      * @return The state object
      */
      DukState* GetState() const;

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

      /**
      * @brief Used to remove stashed objects after they lose all their
      *        references
      */
      static duk_ret_t Finalize(duk_context* ctx);

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
    inline void DukWrapper::PushValue(T value, if_number<T>*) const
    {
      PushValueImpl<double>(static_cast<double>(value));
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void DukWrapper::PushValue(T value, if_enum<T>*) const
    {
      PushValueImpl<int>(static_cast<int>(value));
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

      if (duk_get_prop_string(ctx, -1, DUK_HIDDEN_NAME) > 0)
      {
        bool is_same = strcmp(duk_get_string(ctx, -1), T::ScriptName()) == 0;

        if (is_same && duk_get_prop_string(ctx, -2, DUK_HIDDEN_PTR) > 0)
        {
          callee = duk_get_pointer(ctx, -1);
        }

        duk_pop(ctx);
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
