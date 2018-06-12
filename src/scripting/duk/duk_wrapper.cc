#include "scripting/duk/duk_wrapper.h"

#include "scripting/script_args.h"
#include "scripting/script_class.h"

#include <foundation/auxiliary/logger.h>
#include <foundation/memory/memory.h>

namespace snuffbox
{
  namespace scripting
  {
    //--------------------------------------------------------------------------
    DukWrapper::DukWrapper(duk_context* ctx) :
      context_(ctx)
    {
      foundation::Logger::Assert(
        context_ != nullptr,
        "DukWrapper context_ cannnot be nullptr");
    }

    //--------------------------------------------------------------------------
    void DukWrapper::PushPointer(
      ScriptClass* ptr, 
      const char* type) const
    {
      if (ptr == nullptr)
      {
        duk_push_undefined(context_);
        return;
      }

      if (PushStashedObject(ptr->script_id()) == true)
      {
        return;
      }

      duk_idx_t obj = duk_push_object(context_);

      duk_get_global_string(context_, type);

      if (duk_get_prop_string(context_, -1, "prototype") <= 0)
      {
        duk_pop(context_);
        duk_dup(context_, -1);
      }

      duk_enum(context_, -1, DUK_ENUM_INCLUDE_HIDDEN);

      while (duk_next(context_, -1, 1) > 0)
      {
        duk_put_prop_string(context_, obj, duk_get_string(context_, -2));
        duk_pop(context_);
      }

      duk_pop_n(context_, 3);

      duk_push_pointer(context_, ptr);
      duk_put_prop_string(context_, obj, DUK_HIDDEN_PTR);

      duk_push_string(context_, type);
      duk_put_prop_string(context_, obj, DUK_HIDDEN_NAME);

      ptr->set_state(GetState());
      StashObject(ptr->script_id(), ptr->is_from_script());
    }

    //--------------------------------------------------------------------------
    void DukWrapper::StashObject(
      size_t id, 
      bool from_script, 
      duk_idx_t stack_idx) const
    {
      duk_push_global_stash(context_);

      void* hptr = duk_get_heapptr(context_, stack_idx - 1);

      duk_push_object(context_);
      duk_push_pointer(context_, hptr);
      duk_put_prop_string(context_, -2, DUK_HIDDEN_PTR);

      if (from_script == false)
      {
        duk_dup(context_, stack_idx - 2);
        duk_put_prop_string(context_, -2, DUK_HIDDEN_REF);
      }
      
      foundation::String string_id = foundation::StringUtils::ToString(id);
      duk_put_prop_string(context_, -2, string_id.c_str());
      
      duk_pop(context_);
    }

    //--------------------------------------------------------------------------
    bool DukWrapper::PushStashedObject(size_t id) const
    {
      duk_push_global_stash(context_);

      foundation::String string_id = foundation::StringUtils::ToString(id);
      if (duk_get_prop_string(context_, -1, string_id.c_str()) > 0)
      {
        duk_get_prop_string(context_, -1, DUK_HIDDEN_PTR);
        void* hptr = duk_get_pointer(context_, -1);

        duk_pop_3(context_);
        duk_push_heapptr(context_, hptr);

        return true;
      }

      duk_pop_2(context_);

      return false;
    }

    //--------------------------------------------------------------------------
    void DukWrapper::RemoveStashedObject(size_t id) const
    {
      foundation::String string_id = foundation::StringUtils::ToString(id);
      
      duk_push_global_stash(context_);
      if (duk_get_prop_string(context_, -1, string_id.c_str()) <= 0)
      {
        duk_pop(context_);
        return;
      }

      duk_get_prop_string(context_, -1, DUK_HIDDEN_PTR);
      void* hptr = duk_get_pointer(context_, -1);

      duk_push_heapptr(context_, hptr);
      duk_push_pointer(context_, nullptr);
      duk_put_prop_string(context_, -2, DUK_HIDDEN_PTR);

      duk_pop_3(context_);

      duk_del_prop_string(context_, -1, string_id.c_str());

      duk_pop(context_);
    }

    //--------------------------------------------------------------------------
    void DukWrapper::SetState(DukState* state) const
    {
      duk_push_global_stash(context_);
      duk_push_pointer(context_, state);
      duk_put_prop_string(context_, -2, DUK_HIDDEN_STATE);
      duk_pop(context_);
    }

    //--------------------------------------------------------------------------
    DukState* DukWrapper::GetState() const
    {
      duk_push_global_stash(context_);

      duk_get_prop_string(context_, -1, DUK_HIDDEN_STATE);
      void* ptr = duk_to_pointer(context_, -1);

      duk_pop_2(context_);

      return reinterpret_cast<DukState*>(ptr);
    }

    //--------------------------------------------------------------------------
    template <>
    void DukWrapper::PushValueImpl(bool value) const
    {
      duk_push_boolean(context_, value);
    }

    //--------------------------------------------------------------------------
    template <>
    void DukWrapper::PushValueImpl(double value) const
    {
      duk_push_number(context_, value);
    }

    //--------------------------------------------------------------------------
    template <>
    void DukWrapper::PushValueImpl(const char* value) const
    {
      duk_push_string(context_, value);
    }

    //--------------------------------------------------------------------------
    template <>
    void DukWrapper::PushValueImpl(foundation::String value) const
    {
      PushValueImpl<const char*>(value.c_str());
    }

    //--------------------------------------------------------------------------
    template <>
    void DukWrapper::PushValueImpl(ScriptHandle value) const;

    //--------------------------------------------------------------------------
    template <>
    void DukWrapper::PushValueImpl(ScriptObject* value) const
    {
      ScriptClass* ptr = value->ptr();

      if (ptr != nullptr)
      {
        PushPointer(ptr, value->ptr_type().c_str());
      }
      else
      {
        duk_push_object(context_);
      }

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
    void DukWrapper::PushValueImpl(ScriptArray* value) const
    {
      duk_push_array(context_);

      size_t size = value->size();

      for (size_t i = 0; i < size; ++i)
      {
        PushValueImpl(value->at(i));
        duk_put_prop_index(context_, -2, static_cast<duk_uarridx_t>(i));
      }
    }

    //--------------------------------------------------------------------------
    template <>
    void DukWrapper::PushValueImpl(ScriptValue* value) const
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
    void DukWrapper::PushValueImpl(ScriptHandle value) const
    {
      return PushValueImpl<ScriptValue*>(value.get());
    }

    //--------------------------------------------------------------------------
    ScriptHandle DukWrapper::GetValueAt(duk_idx_t stack_idx) const
    {
      duk_int_t type = duk_get_type(context_, stack_idx);

      switch (type)
      {

      case DUK_TYPE_NULL:
        return foundation::Memory::ConstructShared<ScriptNull>(
          &foundation::Memory::default_allocator());

      case DUK_TYPE_NUMBER:
        return foundation::Memory::ConstructShared<ScriptNumber>(
          &foundation::Memory::default_allocator(),
          duk_to_number(context_, stack_idx));

      case DUK_TYPE_BOOLEAN:
        return foundation::Memory::ConstructShared<ScriptBoolean>(
          &foundation::Memory::default_allocator(),
          duk_to_boolean(context_, stack_idx));

      case DUK_TYPE_STRING:
        return foundation::Memory::ConstructShared<ScriptString>(
          &foundation::Memory::default_allocator(),
          duk_to_string(context_, stack_idx));

      case DUK_TYPE_OBJECT:
        return GetObjectAt(stack_idx);

      default:
        return foundation::Memory::ConstructShared<ScriptNull>(
          &foundation::Memory::default_allocator());

      }
    }

    //--------------------------------------------------------------------------
    ScriptHandle DukWrapper::GetObjectAt(duk_idx_t stack_idx) const
    {
      if (duk_is_function(context_, stack_idx) > 0)
      {
        return foundation::Memory::ConstructShared<ScriptObject>(
          &foundation::Memory::default_allocator());
      }

      auto EnumerateArray = [=]()
      {
        duk_enum(
          context_, 
          -1, 
          DUK_ENUM_ARRAY_INDICES_ONLY | DUK_ENUM_SORT_ARRAY_INDICES);
      };
      
      EnumerateArray();

      size_t array_count = 0;

      while (duk_next(context_, -1, 0) > 0)
      {
        ++array_count;
        duk_pop(context_);
      }

      duk_pop(context_);

      bool is_array = array_count > 0;

      if (is_array == true)
      {
        ScriptArray* arr = 
          foundation::Memory::Construct<ScriptArray>(
            &foundation::Memory::default_allocator());

        arr->resize(array_count);

        array_count = 0;

        EnumerateArray();

        while (duk_next(context_, -1, 1) > 0)
        {
          arr->at(array_count) = GetValueAt(-1);

          duk_pop_2(context_);
          ++array_count;
        }

        duk_pop(context_);

        return foundation::Memory::MakeShared<ScriptArray>(arr);
      }

      ScriptObject* obj =
        foundation::Memory::Construct<ScriptObject>(
          &foundation::Memory::default_allocator());

      if (duk_get_prop_string(context_, stack_idx, DUK_HIDDEN_PTR) > 0)
      {
        duk_get_prop_string(context_, -2, DUK_HIDDEN_NAME);

        obj->SetPointer(
          reinterpret_cast<ScriptClass*>(duk_get_pointer(context_, -2)),
          duk_get_string(context_, -1));

        duk_pop_2(context_);

        return foundation::Memory::MakeShared<ScriptObject>(obj);
      }

      duk_enum(context_, stack_idx, 0);

      const char* key;

      while (duk_next(context_, -1, 1) > 0)
      {
        key = duk_get_string(context_, -2);

        obj->emplace(
          eastl::pair<foundation::String, ScriptHandle>
        {
          key, GetValueAt(-1)
        });

        duk_pop_2(context_);
      }

      duk_pop(context_);

      return foundation::Memory::MakeShared<ScriptObject>(obj);
    }
  }
}