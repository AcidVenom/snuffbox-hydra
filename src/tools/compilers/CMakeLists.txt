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
  "compilers/model_compiler.h"
)

SET(DefinitionsSources
  "definitions/magic.h"
  "definitions/asset_types.h"
  "definitions/asset_types.cc"
  "definitions/default_assets.h"
)

SET(DefinitionsDefaultAssetsSources
  "definitions/default-assets/default_vertex_shader.h"
  "definitions/default-assets/default_pixel_shader.h"
  "definitions/default-assets/default_geometry_shader.h"
)

SET(UtilsSources
  "utils/glslang.h"
  "utils/glslang.cc"
  "utils/gltf_compiler.h"
  "utils/gltf_compiler.cc"
)

IF (SNUFF_WIN32)
  SET(UtilsSources
    ${UtilsSources}
    "utils/hlsl_compiler.h"
    "utils/hlsl_compiler.cc"
  )
ENDIF ()

SOURCE_GROUP("compilers"                    FILES ${CompilersSources})
SOURCE_GROUP("definitions"                  FILES ${DefinitionsSources})
SOURCE_GROUP("definitions\\default-assets"  FILES ${DefinitionsDefaultAssetsSources})
SOURCE_GROUP("utils"                        FILES ${UtilsSources})

SET(ProjectSources
  ${CompilersSources}
  ${DefinitionsSources}
  ${DefinitionsDefaultAssetsSources}
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