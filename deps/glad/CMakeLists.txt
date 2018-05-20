SET(IncludeGladSources
  "include/glad/glad.h"
)

SET(SrcSources
  "src/glad.c"
)

SOURCE_GROUP("include\\glad"  FILES ${IncludeGladSources})
SOURCE_GROUP("src"            FILES ${SrcSources})

SET(GladSources
  ${IncludeGladSources}
  ${SrcSources}
)

ADD_LIBRARY(glad ${GladSources})
TARGET_INCLUDE_DIRECTORIES(glad PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" PRIVATE "include")

