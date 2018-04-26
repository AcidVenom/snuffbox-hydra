SET(RootSources
  "builder.h"
  "builder.cc"
)

SET(DefinitionsSources
  "definitions/magic.h"
)

SET(ThreadingSources
  "threading/build_job.h"
  "threading/build_job.cc"
  "threading/build_scheduler.h"
  "threading/build_scheduler.cc"
)

SET(CompilersSources
  "compilers/compiler.h"
  "compilers/compiler.cc"
  "compilers/script_compiler.h"
  "compilers/script_compiler.cc"
)

SOURCE_GROUP("\\"           FILES ${RootSources})
SOURCE_GROUP("definitions"  FILES ${DefinitionsSources})
SOURCE_GROUP("threading"    FILES ${ThreadingSources})
SOURCE_GROUP("compilers"    FILES ${CompilersSources})

SET(BuilderSources
  ${RootSources}
  ${DefinitionsSources}
  ${ThreadingSources}
  ${CompilersSources}
)

ADD_LIBRARY(snuffbox-builder ${BuilderSources})
TARGET_LINK_LIBRARIES(snuffbox-builder snuffbox-foundation)