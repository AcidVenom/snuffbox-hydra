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

SET(AssetBrowserSources
  "asset-browser/asset_browser.h"
  "asset-browser/asset_browser.cc"
  "asset-browser/asset_icon.h"
  "asset-browser/asset_icon.cc"
  "asset-browser/asset_browser_item.h"
  "asset-browser/asset_browser_item.cc"
)

SET(EditorWidgetsSources
  "editor-widgets/game_view.h"
  "editor-widgets/game_view.cc"
  "editor-widgets/console_widget.h"
  "editor-widgets/console_widget.cc"
)

SET(InputSources
  "input/editor_event_filter.h"
  "input/editor_event_filter.cc"
)

SET(ProjectSources
  "project/project.h"
  "project/project.cc"
  "project/project_window.h"
  "project/project_window.cc"
)

SET(WindowsSources
  "windows/message_box.h"
  "windows/message_box.cc"
  "windows/main_window.h"
  "windows/main_window.cc"
)

SOURCE_GROUP("\\"             FILES ${RootSources})
SOURCE_GROUP("application"    FILES ${ApplicationSources})
SOURCE_GROUP("asset-browser"  FILES ${AssetBrowserSources})
SOURCE_GROUP("editor-widgets" FILES ${EditorWidgetsSources})
SOURCE_GROUP("input"          FILES ${InputSources})
SOURCE_GROUP("project"        FILES ${ProjectSources})
SOURCE_GROUP("windows"        FILES ${WindowsSources})

SET(EditorSources
  ${RootSources}
  ${ApplicationSources}
  ${AssetBrowserSources}
  ${EditorWidgetsSources}
  ${InputSources}
  ${ProjectSources}
  ${WindowsSources}
)

ADD_EXECUTABLE(snuffbox-editor ${EditorSources})
TARGET_LINK_LIBRARIES(snuffbox-editor Qt5::Widgets snuffbox-engine snuffbox-builder)
TARGET_INCLUDE_DIRECTORIES(snuffbox-editor PRIVATE ${CMAKE_CURRENT_BINARY_DIR})