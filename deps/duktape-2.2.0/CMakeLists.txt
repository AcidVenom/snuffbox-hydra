SET(DukSources
  "duk_config.h"
  "duktape.h"
  "duktape.c"
)

ADD_LIBRARY(duktape ${DukSources})
TARGET_INCLUDE_DIRECTORIES(duktape PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})