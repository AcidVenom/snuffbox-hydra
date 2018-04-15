#include "scripting/duk/duk_wrapper.h"

#include "scripting/script_args.h"

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
      auto EnumerateArray = [=]()
      {
        duk_enum(
          context_, 
          stack_idx, 
          DUK_ENUM_ARRAY_INDICES_ONLY | DUK_ENUM_SORT_ARRAY_INDICES);
      };
      
      EnumerateArray();

      size_t array_count = 0;

      while (duk_next(context_, -1, 0) > 0)
      {
        ++array_count;
        duk_pop(context_);
      }

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

      duk_enum(context_, stack_idx, 0);

      ScriptObject* obj =
        foundation::Memory::Construct<ScriptObject>(
          &foundation::Memory::default_allocator());

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

      return foundation::Memory::MakeShared<ScriptObject>(obj);
    }
  }
}