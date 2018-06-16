SET(CompilersSources
  "compilers/compiler.h"
  "compilers/compiler.cc"
  "compilers/script_compiler.h"
  "compilers/script_compiler.cc"
  "compilers/scene_compiler.h"
  "compilers/scene_compiler.cc"
  "compilers/shader_compiler.h"
  "compilers/shader_compiler.cc"
)

SET(DefinitionsSources
  "definitions/magic.h"
  "definitions/asset_types.h"
  "definitions/asset_types.cc"
)

SET(UtilsSources
  "utils/glslang.h"
  "utils/glslang.cc"
)

SOURCE_GROUP("compilers"    FILES ${CompilersSources})
SOURCE_GROUP("definitions"  FILES ${DefinitionsSources})
SOURCE_GROUP("utils"        FILES ${UtilsSources})

SET(ProjectSources
  ${CompilersSources}
  ${DefinitionsSources}
  ${UtilsSources}
)

ADD_LIBRARY(snuffbox-compilers ${ProjectSources})
TARGET_LINK_LIBRARIES(snuffbox-compilers snuffbox-foundation glslang spirv-cross-lib)