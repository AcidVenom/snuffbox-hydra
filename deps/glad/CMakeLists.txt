SET(IncludeGladSources
  "include/glad/glad.h"
)

SET(IncludeKHRSources
  "include/KHR/khrplatform.h"
)

SET(SrcSources
  "src/glad.c"
)

SOURCE_GROUP("include\\glad"  FILES ${IncludeGladSources})
SOURCE_GROUP("include\\KHR"   FILES ${IncludeKHRSources})
SOURCE_GROUP("src"            FILES ${SrcSources})

SET(GladSources
  ${IncludeGladSources}
  ${IncludeKHRSources}
  ${SrcSources}
)

ADD_LIBRARY(glad ${GladSources})
TARGET_INCLUDE_DIRECTORIES(glad PUBLIC "${CMAKE_CURRENT_SOURCE_DIRECTORY}/include")
TARGET_INCLUDE_DIRECTORIES(glad PRIVATE "include")

