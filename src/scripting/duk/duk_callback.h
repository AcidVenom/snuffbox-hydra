#pragma once

#include "scripting/duk/duk_state.h"

#include <foundation/containers/string.h>

#include <duktape.h>

namespace snuffbox
{
  namespace scripting
  {
    /**
    * @brief Used to call scripting functions from the C++ environment
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
      bool Call(Args&&... args);

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
      DukCallback<Args...> cb = DukCallback<Args...>(name, state);

      duk_context* ctx = state->context();

      const char* func_name = name.c_str();

      duk_push_global_stash(ctx);
      duk_push_global_object(ctx);

      bool res = duk_get_prop_string(ctx, -1, func_name);

      if (res == false)
      {
        if (func != nullptr)
        {
          *func = cb;
        }

        duk_pop_3(ctx);

        return false;
      }

      cb.is_valid_ = true;

      duk_put_prop_string(ctx, -3, func_name);

      duk_pop_2(ctx);

      if (func != nullptr)
      {
        *func = cb;
      }

      return true;
    }

    //--------------------------------------------------------------------------
    template <typename ... Args>
    inline bool DukCallback<Args...>::Call(Args&&... args)
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

      bool has_error = false;
      if ((has_error = duk_pcall(ctx, 0)) != 0)
      {
        state_->LogLastError("Callback error in ({0}:{1}):\n\n{2}");
      }

      duk_pop_2(ctx);

      return has_error;
    }
  }
}