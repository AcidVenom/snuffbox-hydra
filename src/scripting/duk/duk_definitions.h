#pragma once

namespace snuffbox
{
  namespace scripting
  {
#define DUK_HIDDEN_CALLBACK DUK_HIDDEN_SYMBOL("__callback")
#define DUK_HIDDEN_PTR DUK_HIDDEN_SYMBOL("__ptr")
#define DUK_HIDDEN_NAME DUK_HIDDEN_SYMBOL("__name")
  }
}