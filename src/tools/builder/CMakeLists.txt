SET(RootSources
  "builder.h"
  "builder.cc"
)

SET(ThreadingSources
  "threading/build_job.h"
  "threading/build_job.cc"
  "threading/build_item.h"
  "threading/build_item.cc"
  "threading/build_scheduler.h"
  "threading/build_scheduler.cc"
)

SOURCE_GROUP("\\"           FILES ${RootSources})
SOURCE_GROUP("threading"    FILES ${ThreadingSources})
SOURCE_GROUP("compilers"    FILES ${CompilersSources})

SET(BuilderSources
  ${RootSources}
  ${DefinitionsSources}
  ${ThreadingSources}
  ${CompilersSources}
)

ADD_LIBRARY(snuffbox-builder ${BuilderSources})
TARGET_LINK_LIBRARIES(snuffbox-builder snuffbox-compilers)