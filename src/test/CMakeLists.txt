SET(RootSources
  "main.cc"
)

SOURCE_GROUP("\\" FILES ${RootSources})

SET(TestSources
  ${RootSources}
)

ADD_EXECUTABLE(snuffbox-test ${TestSources})
TARGET_LINK_LIBRARIES(snuffbox-test snuffbox-engine)