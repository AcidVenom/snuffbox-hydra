SET(ApplicationSources
  "application/application.h"
  "application/application.cc"
  "application/window.h"
  "application/window.cc"
)

SOURCE_GROUP("application" FILES ${ApplicationSources})

SET(EngineSources
  ${ApplicationSources}
)

ADD_LIBRARY(snuffbox-engine ${EngineSources})
TARGET_LINK_LIBRARIES(snuffbox-engine snuffbox-foundation)