SET(DefinitionsSources
  "definitions/cli.h"
  "definitions/keycodes.h"
  "definitions/input.h"
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
  "services/window_service.h"
  "services/window_service.cc"
  "services/input_service.h"
  "services/input_service.cc"
)

IF (NOT SNUFF_NSCRIPTING)
  SET(ServicesSources ${ServicesSources}
    "services/script_service.h"
    "services/script_service.cc"
  )
ENDIF ()

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

SET(InputSources
  "input/input_filter.h"
  "input/input_filter.cc"
  "input/input_event.h"
  "input/input_event.cc"
  "input/input_event_handler.h"
  "input/input_event_handler.cc"
  "input/keyboard.h"
  "input/keyboard.cc"
  "input/mouse.h"
  "input/mouse.cc"
  "input/joystick.h"
  "input/joystick.cc"
)

SET(Resources
)

SOURCE_GROUP("definitions"  FILES ${DefinitionsSources})
SOURCE_GROUP("auxiliary"    FILES ${AuxiliarySources})
SOURCE_GROUP("services"     FILES ${ServicesSources})
SOURCE_GROUP("application"  FILES ${ApplicationSources})
SOURCE_GROUP("cvar"         FILES ${CVarSources})
SOURCE_GROUP("input"        FILES ${InputSources})
SOURCE_GROUP("rsc"          FILES ${Resources})

SET(EngineSources
  ${DefinitionsSources}
  ${AuxiliarySources}
  ${ServicesSources}
  ${ApplicationSources}
  ${CVarSources}
  ${InputSources}
  ${Resources}
)

ADD_LIBRARY(snuffbox-engine ${EngineSources})
TARGET_LINK_LIBRARIES(snuffbox-engine snuffbox-builder glfw)

ADD_DEPENDENCIES(snuffbox-engine snuffbox-bin2h)

SET(WorkingDir ${PROJECT_BINARY_DIR}/generated)

FILE(MAKE_DIRECTORY ${WorkingDir})
FILE(MAKE_DIRECTORY ${WorkingDir}/resources)

SNUFF_BIN2H("${Resources}" ${WorkingDir}/resources)

TARGET_INCLUDE_DIRECTORIES(snuffbox-engine PRIVATE ${WorkingDir})

IF (NOT SNUFF_NSCRIPTING)
  TARGET_LINK_LIBRARIES(snuffbox-engine snuffbox-scripting)
  ADD_DEPENDENCIES(snuffbox-engine snuffbox-sparse)
  
  SET(ScriptingBinds
    "auxiliary/debug.h"
    "services/input_service.h"
    "definitions/keycodes.h"
  )
  
  FILE(MAKE_DIRECTORY ${WorkingDir}/sparsed)
  
  CREATE_SCRIPT_BINDS("${ScriptingBinds}" ${WorkingDir}/sparsed)
ENDIF (NOT SNUFF_NSCRIPTING)
