#pragma once

#include "scripting/script_type_traits.h"
#include "scripting/script_value.h"

#include <foundation/containers/vector.h>

#include <duktape.h>

#include <string>

namespace snuffbox
{
  namespace scripting
  {
    class ScriptArgs;

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
      * @param[out] args The converted arguments
      */
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
    template <>
    inline void DukWrapper::PushValueImpl(bool value) const
    {
      duk_push_boolean(context_, value);
    }

    //--------------------------------------------------------------------------
    template <>
    inline void DukWrapper::PushValueImpl(double value) const
    {
      duk_push_number(context_, value);
    }

    //--------------------------------------------------------------------------
    template <>
    inline void DukWrapper::PushValueImpl(const char* value) const
    {
      duk_push_string(context_, value);
    }

    //--------------------------------------------------------------------------
    template <>
    inline void DukWrapper::PushValueImpl(foundation::String value) const
    {
      PushValueImpl<const char*>(value.c_str());
    }

    //--------------------------------------------------------------------------
    template <>
    inline void DukWrapper::PushValueImpl(ScriptHandle value) const;

    //--------------------------------------------------------------------------
    template <>
    inline void DukWrapper::PushValueImpl(ScriptObject* value) const
    {
      duk_push_object(context_);

      for (
        ScriptObject::iterator it = value->begin();
        it != value->end();
        ++it)
      {
        PushValueImpl(it->second);
        duk_put_prop_string(context_, -2, it->first.c_str());
      }
    }

    //--------------------------------------------------------------------------
    template <>
    inline void DukWrapper::PushValueImpl(ScriptArray* value) const
    {
      duk_push_array(context_);

      for (size_t i = 0; i < value->size(); ++i)
      {
        PushValueImpl(value->at(i));
        duk_put_prop_string(context_, -2, std::to_string(i).c_str());
      }
    }

    //--------------------------------------------------------------------------
    template <>
    inline void DukWrapper::PushValueImpl(ScriptValue* value) const
    {
      switch (value->type())
      {
      case ScriptValueTypes::kNull:
        duk_push_undefined(context_);
        break;

      case ScriptValueTypes::kNumber:
        PushValueImpl(
          static_cast<ScriptNumber*>(value)->value());
        break;

      case ScriptValueTypes::kBoolean:
        PushValueImpl(
          static_cast<ScriptBoolean*>(value)->value());
        break;

      case ScriptValueTypes::kString:
        PushValueImpl(
          static_cast<ScriptString*>(value)->value());
        break;

      case ScriptValueTypes::kObject:
        PushValueImpl(
          static_cast<ScriptObject*>(value));
        break;

      case ScriptValueTypes::kArray:
        PushValueImpl(
          static_cast<ScriptArray*>(value));
        break;

      default:
        duk_push_undefined(context_);
        break;
      }
    }

    //--------------------------------------------------------------------------
    template <>
    inline void DukWrapper::PushValueImpl(ScriptHandle value) const
    {
      return PushValueImpl<ScriptValue*>(value.get());
    }
  }
}