SET(CompilersSources
  "compilers/compiler.h"
  "compilers/compiler.cc"
  "compilers/script_compiler.h"
  "compilers/script_compiler.cc"
  "compilers/scene_compiler.h"
  "compilers/scene_compiler.cc"
  "compilers/shader_compiler.h"
  "compilers/shader_compiler.cc"
  "compilers/material_compiler.h"
  "compilers/material_compiler.cc"
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

IF (SNUFF_WIN32)
  SET(UtilsSources
    ${UtilsSources}
    "utils/hlsl_compiler.h"
    "utils/hlsl_compiler.cc"
  )
ENDIF ()

SOURCE_GROUP("compilers"    FILES ${CompilersSources})
SOURCE_GROUP("definitions"  FILES ${DefinitionsSources})
SOURCE_GROUP("utils"        FILES ${UtilsSources})

SET(ProjectSources
  ${CompilersSources}
  ${DefinitionsSources}
  ${UtilsSources}
)

ADD_LIBRARY(snuffbox-compilers ${ProjectSources})
TARGET_LINK_LIBRARIES(snuffbox-compilers snuffbox-foundation glslang-lib spirv-cross-lib tinygltf)

IF (SNUFF_WIN32)
  TARGET_LINK_LIBRARIES(snuffbox-compilers d3dcompiler)
ENDIF (SNUFF_WIN32)

IF (NOT SNUFF_NSCRIPTING)

  SET(WorkingDir ${PROJECT_BINARY_DIR}/generated)
  FILE(MAKE_DIRECTORY ${WorkingDir})
  TARGET_INCLUDE_DIRECTORIES(snuffbox-compilers PRIVATE ${WorkingDir})

  ADD_DEPENDENCIES(snuffbox-compilers snuffbox-sparse)
  
  SET(ScriptingBinds
    "definitions/asset_types.h"
  )
  
  FILE(MAKE_DIRECTORY ${WorkingDir}/sparsed)
  
  CREATE_SCRIPT_BINDS("${ScriptingBinds}" ${WorkingDir}/sparsed)
ENDIF (NOT SNUFF_NSCRIPTING)