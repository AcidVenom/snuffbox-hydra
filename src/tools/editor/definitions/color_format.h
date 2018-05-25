#pragma once

#include <qcolor.h>

namespace snuffbox
{
  namespace editor
  {
    /**
    * @brief Used to contain a foreground and background color
    *
    * @author Daniel Konings
    */
    struct ColorFormat
    {
      QColor foreground; //!< The foreground color
      QColor background; //!< The background color
    };
  }
}