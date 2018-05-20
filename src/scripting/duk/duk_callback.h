#pragma once

#include "scripting/duk/duk_state.h"
#include "scripting/duk/duk_wrapper.h"

#include <foundation/containers/string.h>

#include <cinttypes>

namespace snuffbox
{
  namespace scripting
  {
    /**
    * @brief Used to call scripting functions from the C++ environment
    *
    * @tparam Args... The arguments to call the callback with
    *
    * The callback doesn't have an interface, as it is fully templated. The
    * functions of one callback implementation should match the other.
    *
    * @author Daniel Konings
    */
    template <typename ... Args>
    class DukCallback
    {

    public:

      /**
      * @brief Default constructor, invalidates the callback
      */
      DukCallback();

    protected:

      /**
      * @brief Constructs a callback by name, by retrieving its value from
      *        the global duktape scope
      *
      * Unlike other scripting APIs, there is no actual reference to the
      * callback. The name is stored so that it can later be retrieved from
      * duktape's global stash.
      *
      * @see http://duktape.org/api.html#concepts.10
      *
      * @param[in] name The name of the callback in the global scope
      * @param[in] state The current duktape state
      */
      DukCallback(const foundation::String& name, DukState* state);

    public:

      /**
      * @brief Creates a callback from a global function
      *
      * @param[in] state The duktape state
      * @param[in] name The name of the function in the global scope
      * @param[out] func The created callback, if not nullptr
      *
      * @return Could we find the function in the global scope?
      */
      static bool FromGlobal(
        DukState* state, 
        const foundation::String& name,
        DukCallback<Args...>* func);

      /**
      * @brief Creates a callback from an object
      *
      * @param[in] state The duktape state
      * @param[in] object The name of the object in the global scope
      * @param[in] name The name of the function in the object
      * @param[out] func The created callback, if not nullptr
      *
      * @return Could we find the function?
      */
      static bool FromClass(
        DukState* state,
        const foundation::String& object,
        const foundation::String& name,
        DukCallback<Args...>* func);

    protected:

      /**
      * @brief Creates a callback from a stack index
      *
      * @see DukCallback::FromGlobal
      * @see DukCallback::FromClass
      *
      * @param[in] stack_idx The index of the callback object
      */
      static bool FromStackIndex(
        DukState* state,
        const foundation::String& name,
        const foundation::String& store_name,
        DukCallback<Args...>* func,
        duk_idx_t stack_idx = -1);

      /**
      * @brief Variadic unroll of the input arguments for the DukWrapper
      *        to push them as scripting values onto the stack
      *
      * @tparam T The type of the current value
      * @tparam Others The other arguments left to unroll
      *
      * @remarks Called when there is still more than 1 argument left
      *
      * @param[in] wrapper The wrapper object
      * @param[in] current The current value
      * @param[in] args The other arguments
      *
      * @return The number of unrolled arguments
      */
      template <typename T, typename ... Others>
      uint8_t PushArg(DukWrapper& wrapper, T current, Others&&... args);


      /**
      * @see DukCallback::PushArg
      *
      * @remarks The last function call of the unroll
      */
      template <typename T>
      uint8_t PushArg(DukWrapper& wrapper, T last);

      /**
      * @see DukCallback::PushArg
      *
      * @remarks Called when there are no arguments initially
      */
      uint8_t PushArg(DukWrapper& wrapper);

    public:

      /**
      * @brief Call the callback with specified C++ arguments
      *
      * The arguments will be converted to their script values by the native
      * implementation and then call the corresponding scripting variant that
      * was registered.
      *
      * @param[in] args The arguments to call the callback with
      *
      * @return Was the call a success?
      */
      bool Call(Args... args);

      /**
      * @see DukCallback::Call
      *
      * @tparam T The ScriptClass that calls this callback
      *
      * @brief Calls the callback with a context (for instance, a self or this)
      *
      * @remarks An object containing the pointer will be set as the "this"
      *          object of the called function
      *
      * @param[in] self The callee that calls this function
      * @param[in] args The arguments to call the callback with
      */
      template <typename T>
      bool CallContext(T* self, Args... args);

      /**
      * @brief Pushes the callback from the global stash
      *
      * @param[out] nargs The number of arguments for this callback
      * @param[out] top_args The stack index between the callback and the
      *                      arguments, used for context calls
      * @param[in] args The arguments to call the callback with
      *
      * @return Were we able to push the callback onto the stack?
      */
      bool PushCallback(uint8_t* nargs, duk_idx_t* top_args, Args... args);

    private:

      foundation::String name_; //!< The name of this callback
      DukState* state_; //!< The state this callback was created in

      bool is_valid_; //!< Is this callback valid?
    };

    //--------------------------------------------------------------------------
    template <typename ... Args>
    inline DukCallback<Args...>::DukCallback() :
      name_(""),
      state_(nullptr),
      is_valid_(false)
    {
      
    }

    //--------------------------------------------------------------------------
    template <typename ... Args>
    inline DukCallback<Args...>::DukCallback(
      const foundation::String& name,
      DukState* state) 
      :
      name_(name),
      state_(state),
      is_valid_(false)
    {

    }

    //--------------------------------------------------------------------------
    template <typename ... Args>
    inline bool DukCallback<Args...>::FromGlobal(
      DukState* state,
      const foundation::String& name,
      DukCallback<Args...>* func)
    {
      duk_context* ctx = state->context();

      duk_push_global_object(ctx);
      bool found = FromStackIndex(state, name, name, func);
      duk_pop(ctx);

      return found;
    }

    //--------------------------------------------------------------------------
    template <typename ... Args>
    inline bool DukCallback<Args...>::FromClass(
      DukState* state,
      const foundation::String& object,
      const foundation::String& name,
      DukCallback<Args...>* func)
    {
      duk_context* ctx = state->context();

      duk_push_global_object(ctx);
      if (duk_get_prop_string(ctx, -1, object.c_str()) <= 0)
      {
        duk_pop_2(ctx);
        return false;
      }

      bool found = FromStackIndex(state, name, object + "." + name, func);
      duk_pop_2(ctx);

      return found;
    }

    //--------------------------------------------------------------------------
    template <typename ... Args>
    inline bool DukCallback<Args...>::FromStackIndex(
      DukState* state,
      const foundation::String& name,
      const foundation::String& store_name,
      DukCallback<Args...>* func,
      duk_idx_t stack_idx)
    {
      duk_context* ctx = state->context();
      DukCallback<Args...> cb = DukCallback<Args...>(store_name, state);

      duk_push_global_stash(ctx);
      bool res = duk_get_prop_string(ctx, stack_idx - 1, name.c_str()) > 0;

      if (res == false || duk_is_function(ctx, -1) <= 0)
      {
        if (func != nullptr)
        {
          *func = cb;
        }

        duk_pop_2(ctx);

        return false;
      }

      cb.is_valid_ = true;

      duk_put_prop_string(ctx, -2, store_name.c_str());

      duk_pop(ctx);

      if (func != nullptr)
      {
        *func = cb;
      }

      return true;
    }

    //--------------------------------------------------------------------------
    template <typename ... Args> 
    template <typename T, typename ... Others>
    inline uint8_t DukCallback<Args...>::PushArg(
      DukWrapper& wrapper, 
      T current, 
      Others&&... args)
    {
      wrapper.PushValue<T>(current);
      return PushArg(wrapper, eastl::forward<Others>(args)...) + 1;
    }

    //--------------------------------------------------------------------------
    template <typename ... Args> 
    template <typename T>
    inline uint8_t DukCallback<Args...>::PushArg(DukWrapper& wrapper, T last)
    {
      wrapper.PushValue<T>(last);
      return 1;
    }

    //--------------------------------------------------------------------------
    template <typename ... Args>
    inline uint8_t DukCallback<Args...>::PushArg(DukWrapper& wrapper)
    {
      return 0;
    }

    //--------------------------------------------------------------------------
    template <typename ... Args>
    inline bool DukCallback<Args...>::Call(Args... args)
    {
      if (state_ == nullptr || is_valid_ == false)
      {
        return false;
      }

      uint8_t nargs;
      if (PushCallback(&nargs, nullptr, eastl::forward<Args>(args)...) == false)
      {
        return false;
      }

      duk_context* ctx = state_->context();

      bool has_error = false;
      if ((has_error = duk_pcall(ctx, nargs)) != 0)
      {
        state_->LogLastError("Callback error in ({0}:{1}):\n\n{2}");
      }

      duk_pop(ctx);

      return has_error;
    }

    //--------------------------------------------------------------------------
    template <typename ... Args>
    template <typename T>
    inline bool DukCallback<Args...>::CallContext(T* self, Args... args)
    {
      if (state_ == nullptr || is_valid_ == false)
      {
        return false;
      }

      duk_context* ctx = state_->context();
      DukWrapper wrapper(ctx);

      uint8_t nargs;
      duk_idx_t top;

      if (PushCallback(&nargs, &top, eastl::forward<Args>(args)...) == false)
      {
        return false;
      }

      wrapper.PushPointer(self, T::ScriptName());
      duk_replace(ctx, top - 1);

      bool has_error = false;
      if ((has_error = duk_pcall_method(ctx, nargs)) != 0)
      {
        state_->LogLastError("Callback error in ({0}:{1}):\n\n{2}");
      }

      duk_pop(ctx);

      return has_error;
    }

    //--------------------------------------------------------------------------
    template <typename ... Args>
    inline bool DukCallback<Args...>::PushCallback(
      uint8_t* nargs, 
      duk_idx_t* top_args, 
      Args... args)
    {
      if (is_valid_ == false || state_ == nullptr)
      {
        return false;
      }

      duk_context* ctx = state_->context();

      duk_push_global_stash(ctx);
      if (duk_get_prop_string(ctx, -1, name_.c_str()) == false)
      {
        duk_pop_2(ctx);
        return false;
      }

      duk_remove(ctx, -2);

      if (top_args != nullptr)
      {
        duk_push_undefined(ctx);
      }

      DukWrapper wrapper = DukWrapper(ctx);

      uint8_t n = PushArg(wrapper, eastl::forward<Args>(args)...);

      if (nargs != nullptr)
      {
        *nargs = n;
      }

      if (top_args != nullptr)
      {
        *top_args = -(static_cast<duk_idx_t>(n) + 1);
      }

      return true;
    }
  }
}