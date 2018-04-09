SET(DukSources
  "duk_config.h"
  "duktape.h"
  "duktape.c"
)

SOURCE_GROUP("\\" FILES ${DukSources})

ADD_LIBRARY(duktape ${DukSources})
TARGET_INCLUDE_DIRECTORIES(duktape PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})