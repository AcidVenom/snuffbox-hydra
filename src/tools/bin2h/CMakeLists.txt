SET(RootSources
  "main.cc"
  "binary_writer.h"
  "binary_writer.cc"
)

SOURCE_GROUP("\\" FILES ${RootSources})

SET(Bin2HSources
  ${RootSources}
)

ADD_EXECUTABLE(snuffbox-bin2h ${Bin2HSources})