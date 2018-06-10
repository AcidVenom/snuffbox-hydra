SET(RootSources
  "compiler.h"
  "compiler.cc"
  "script_compiler.h"
  "script_compiler.cc"
  "scene_compiler.h"
  "scene_compiler.cc"
)

SET(DefinitionsSources
  "definitions/magic.h"
  "definitions/asset_types.h"
  "definitions/asset_types.cc"
)

SOURCE_GROUP("\\"          FILES ${RootSources})
SOURCE_GROUP("definitions" FILES ${DefinitionsSources})

SET(CompilersSources
  ${RootSources}
  ${DefinitionsSources}
)

ADD_LIBRARY(snuffbox-compilers ${CompilersSources})
TARGET_LINK_LIBRARIES(snuffbox-compilers snuffbox-foundation)