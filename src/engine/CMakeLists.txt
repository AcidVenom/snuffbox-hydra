SET(DefinitionsSources
  "definitions/cli.h"
)

SET(AuxiliarySources
  "auxiliary/debug.h"
  "auxiliary/debug.cc"
)

SET(ServicesSources
  "services/service.h"
  "services/service.cc"
  "services/cvar_service.h"
  "services/cvar_service.cc"
)

SET(ApplicationSources
  "application/application.h"
  "application/application.cc"
  "application/window.h"
  "application/window.cc"
)

SET(CVarSources
  "cvar/cvar.h"
  "cvar/cvar.cc"
  "cvar/command_line_parser.h"
  "cvar/command_line_parser.cc"
)

SOURCE_GROUP("definitions"  FILES ${DefinitionsSources})
SOURCE_GROUP("auxiliary"    FILES ${AuxiliarySources})
SOURCE_GROUP("services"     FILES ${ServicesSources})
SOURCE_GROUP("application"  FILES ${ApplicationSources})
SOURCE_GROUP("cvar"         FILES ${CVarSources})

SET(EngineSources
  ${DefinitionsSources}
  ${AuxiliarySources}
  ${ServicesSources}
  ${ApplicationSources}
  ${CVarSources}
)

ADD_LIBRARY(snuffbox-engine ${EngineSources})
TARGET_LINK_LIBRARIES(snuffbox-engine snuffbox-foundation)

IF (NOT SNUFF_NSCRIPTING)
  TARGET_LINK_LIBRARIES(snuffbox-engine snuffbox-scripting)
  ADD_DEPENDENCIES(snuffbox-engine snuffbox-sparse)
  
  SET(ScriptingBinds
    "auxiliary/debug.h"
  )
  
  SET(WorkingDir ${PROJECT_BINARY_DIR}/generated)
  TARGET_INCLUDE_DIRECTORIES(snuffbox-engine PRIVATE ${WorkingDir})
  SET(WorkingDir "${WorkingDir}/sparsed")
  
  FOREACH (Relative ${ScriptingBinds})
    IF(${Relative} MATCHES "\\.h")
      SET(InputFile "${CMAKE_CURRENT_SOURCE_DIR}/${Relative}")
      
      MESSAGE(STATUS "Adding custom compilation step for ${InputFile}")
      
      STRING(REGEX REPLACE ".*\\/" "" RelativePath ${InputFile})
      STRING(REGEX REPLACE "\\.h" "" NoExtension ${RelativePath})
      
      SET(OutputFile "${WorkingDir}/${NoExtension}.gen.cc")
      
      SET(ParseArgs "${InputFile}" -c SCRIPT_CLASS -e SCRIPT_ENUM -f SCRIPT_FUNC -m SCRIPT_NAME)
      SET(ParseTo "${NoExtension}.json")
      
      ADD_CUSTOM_COMMAND(
        OUTPUT ${OutputFile}
        COMMAND "$<TARGET_FILE:header-parser>" ${ParseArgs} > ${ParseTo} && "$<TARGET_FILE:snuffbox-sparse>" -i ${ParseTo} -o ${OutputFile} -h ${InputFile}
        DEPENDS ${InputFile}
        MAIN_DEPENDENCY ${InputFile}
        WORKING_DIRECTORY "${WorkingDir}"
        COMMENT "------ sparse: Generating script bindings for ${InputFile}"
        VERBATIM
      )
    ENDIF ()
  ENDFOREACH ()
ENDIF (NOT SNUFF_NSCRIPTING)