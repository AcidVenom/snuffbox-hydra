SET(CMAKE_AUTOMOC ON)

SET(RootSources
  "main.cc"
)

SET(ApplicationSources
  "application/editor_application.h"
  "application/editor_application.cc"
  "application/styling.h"
  "application/styling.cc"
)

SET(WindowsSources
  "windows/message_box.h"
  "windows/message_box.cc"
  "windows/main_window.h"
  "windows/main_window.cc"
  "windows/game_view.h"
  "windows/game_view.cc"
)

SET(ProjectSources
  "project/project.h"
  "project/project.cc"
  "project/project_window.h"
  "project/project_window.cc"
)

SET(InputSources
  "input/editor_event_filter.h"
  "input/editor_event_filter.cc"
)

SOURCE_GROUP("\\"           FILES ${RootSources})
SOURCE_GROUP("application"  FILES ${ApplicationSources})
SOURCE_GROUP("windows"      FILES ${WindowsSources})
SOURCE_GROUP("project"      FILES ${ProjectSources})
SOURCE_GROUP("input"        FILES ${InputSources})

SET(EditorSources
  ${RootSources}
  ${ApplicationSources}
  ${WindowsSources}
  ${ProjectSources}
  ${InputSources}
)

ADD_EXECUTABLE(snuffbox-editor ${EditorSources})
TARGET_LINK_LIBRARIES(snuffbox-editor Qt5::Widgets snuffbox-engine snuffbox-builder)
TARGET_INCLUDE_DIRECTORIES(snuffbox-editor PRIVATE ${CMAKE_CURRENT_BINARY_DIR})