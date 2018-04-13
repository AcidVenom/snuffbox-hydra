SET(RootSources
  "main.cc"
)

SOURCE_GROUP("\\"     FILES ${RootSources})

SET(SparseSources
  ${RootSources}
)

ADD_EXECUTABLE(snuffbox-sparse ${SparseSources})
TARGET_LINK_LIBRARIES(snuffbox-sparse rapidjson)
ADD_DEPENDENCIES(snuffbox-sparse header-parser)