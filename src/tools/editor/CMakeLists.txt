SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTOMOC ON)

SET(RootSources
  "main.cc"
)

SET(WindowsSources
  "windows/main_window.ui"
  "windows/main_window.h"
  "windows/main_window.cc"
  "windows/console.h"
  "windows/console.cc"
)

SOURCE_GROUP("\\"       FILES ${RootSources})
SOURCE_GROUP("windows"  FILES ${WindowsSources})

SET(EditorSources
  ${RootSources}
  ${WindowsSources}
)

ADD_EXECUTABLE(snuffbox-editor ${EditorSources})
TARGET_LINK_LIBRARIES(snuffbox-editor Qt5::Widgets snuffbox-engine)
TARGET_INCLUDE_DIRECTORIES(snuffbox-editor PRIVATE ${CMAKE_CURRENT_BINARY_DIR})