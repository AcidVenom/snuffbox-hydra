SET(RootSources
  "main.cc"
  "definitions.h"
  "json_header_parser.h"
  "json_header_parser.cc"
)

SOURCE_GROUP("\\"     FILES ${RootSources})

SET(SparseSources
  ${RootSources}
)

ADD_EXECUTABLE(snuffbox-sparse ${SparseSources})
TARGET_LINK_LIBRARIES(snuffbox-sparse rapidjson)
ADD_DEPENDENCIES(snuffbox-sparse header-parser)