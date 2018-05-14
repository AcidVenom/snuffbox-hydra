#pragma once

#include "foundation/memory/allocators/eastl_allocator.h"

#include <EASTL/string.h>

#include <cinttypes>

namespace snuffbox
{
  namespace foundation
  {
    /**
    * @brief An EASTL string with a custom allocator
    */
    using String = eastl::basic_string<char, EASTLAllocator>;
  }
}

namespace eastl
{
  /**
  * @brief Makes sure that our version of EASTL strings with an allocator
  *        can be used in hash maps
  */
  template <>
  struct hash<snuffbox::foundation::String>
  {
    /**
    * @brief Taken straight from EASTL's hash<eastl::string> implementation
    */
    size_t operator()(const snuffbox::foundation::String& x) const
    {
      const uint8_t* p = reinterpret_cast<const uint8_t*>(x.c_str());

      uint32_t c;
      uint32_t result = 2166136261U;

      while ((c = *p++) != 0)
      {
        result = (result * 16777619) ^ c;
      }

      return static_cast<size_t>(result);
    }
  };

  /**
  * @brief Makes sure that our version of EASTL strings with an allocator
  *        can be used in hash maps
  */
  template <>
  struct equal_to<snuffbox::foundation::String> 
  {
    EA_CPP14_CONSTEXPR bool operator()(
      const snuffbox::foundation::String& a, 
      const snuffbox::foundation::String& b) const
    { 
      return a == b; 
    }
  };
}

