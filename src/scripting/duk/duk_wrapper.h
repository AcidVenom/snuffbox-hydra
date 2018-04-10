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
      * @brief Used to push numerical values
      *
      * @see DukWrapper::PushValueImpl
      */
      template <typename T>
      void PushValue(
        T value, 
        if_number<T>* = nullptr) const
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
        if_enum<T>* = nullptr) const
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
        if_n_number_and_enum<T>* = nullptr) const
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
      void PushValueImpl(T value) const;

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

    private:

      duk_context* context_; //!< The context for this wrapper
    };

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
    inline void DukWrapper::PushValueImpl(
      foundation::SharedPtr<ScriptObject> value) const
    {
      duk_push_object(context_);

      ScriptObject* obj = value.get();

      for (
        ScriptObject::iterator it = obj->begin();
        it != obj->end();
        ++it)
      {
        PushValueImpl(it->second);
        duk_put_prop_string(context_, -1, it->first.c_str());
      }
    }

    //--------------------------------------------------------------------------
    template <>
    inline void DukWrapper::PushValueImpl(
      foundation::SharedPtr<ScriptArray> value) const
    {
      duk_push_array(context_);

      ScriptArray* arr = value.get();

      for (size_t i = 0; i < arr->size(); ++i)
      {
        PushValueImpl(arr->at(i));
        duk_put_prop_string(context_, -1, std::to_string(i).c_str());
      }
    }

    //--------------------------------------------------------------------------
    template <>
    inline void DukWrapper::PushValueImpl(ScriptHandle value) const
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