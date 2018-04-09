#pragma once

#include "scripting/script_value.h"

#include <foundation/containers/vector.h>

#include <EASTL/type_traits.h>
#include <duktape.h>

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
      * @brief Used to only enable a function if the template parameter is a
      *        number
      *
      * @tparam T The type to check
      */
      template <typename T>
      using is_number = 
        typename eastl::enable_if<eastl::is_arithmetic<T>::value, T>::type;

      /**
      * @brief Used to only enable a function if the template parameter is an
      *        enumerator
      *
      * @tparam T The type to check
      */
      template <typename T>
      using is_enum = 
        typename eastl::enable_if<eastl::is_enum<T>::value, T>::type;

      /**
      * @brief Used to only enable a function if the template parameter is both
      *        not a number and not an enumerator
      *
      * @tparam T The type to check
      */
      template <typename T>
      using is_n_number_and_enum =
        typename eastl::enable_if<
        eastl::is_arithmetic<T>::value == false &&
        eastl::is_enum<T>::value == false,
        T>::type;

      /**
      * @brief Used to push numerical values
      *
      * @see DukWrapper::PushValueImpl
      */
      template <typename T>
      void PushValue(
        T value, 
        is_number<T>* = nullptr)
      {
        PushValueImpl<double>(static_cast<double>(value));
      }

      /**
      * @brief Used to push enumerator values
      *
      * @see DukWrapper::PushValueImpl
      */
      template <typename T>
      void PushValue( 
        T value, 
        is_enum<T>* = nullptr)
      {
        PushValueImpl<double>(static_cast<double>(value));
      }

      /**
      * @brief Used to push any value that is both not an enumeration,
      *        nor a numerical value
      *
      * @see DukWrapper::PushValueImpl
      */
      template <typename T>
      void PushValue(
        T value,
        is_n_number_and_enum<T>* = nullptr)
      {
        PushValueImpl<T>(value);
      }

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
      void PushValueImpl(T value);

      /**
      * @brief Retrieves a value from the duktape stack as a ScriptHandle
      *
      * @param[in] stack_idx The index of the value to retrieve
      */
      ScriptHandle GetValueAt(int stack_idx);

    private:

      duk_context* context_; //!< The context for this wrapper
    };

    //--------------------------------------------------------------------------
    template <>
    inline void DukWrapper::PushValueImpl(bool value)
    {
      duk_push_boolean(context_, value);
    }

    //--------------------------------------------------------------------------
    template <>
    inline void DukWrapper::PushValueImpl(double value)
    {
      duk_push_number(context_, value);
    }

    //--------------------------------------------------------------------------
    template <>
    inline void DukWrapper::PushValueImpl(const char* value)
    {
      duk_push_string(context_, value);
    }

    //--------------------------------------------------------------------------
    template <>
    inline void DukWrapper::PushValueImpl(foundation::String value)
    {
      PushValueImpl<const char*>(value.c_str());
    }

    //--------------------------------------------------------------------------
    template <>
    inline void DukWrapper::PushValueImpl(ScriptHandle value)
    {
      switch (value->type())
      {
      case ScriptValue::Types::kNull:
        duk_push_undefined(context_);
        break;

      case ScriptValue::Types::kNumber:
        PushValueImpl(
          eastl::static_pointer_cast<ScriptNumber>(value)->value());
        break;

      case ScriptValue::Types::kBoolean:
        PushValueImpl(
          eastl::static_pointer_cast<ScriptBoolean>(value)->value());
        break;

      case ScriptValue::Types::kString:
        PushValueImpl(
          eastl::static_pointer_cast<ScriptString>(value)->value());
        break;

      case ScriptValue::Types::kObject:
        PushValueImpl(
          eastl::static_pointer_cast<ScriptObject>(value));
        break;

      case ScriptValue::Types::kArray:
        PushValueImpl(
          eastl::static_pointer_cast<ScriptArray>(value));
        break;

      default:
        duk_push_undefined(context_);
        break;
      }
    }
  }
}