SET(DefinitionsSources
  "definitions/error_codes.h"
  "definitions/logging.h"
)

SET(MemorySources
  "memory/memory.h"
  "memory/memory.cc"
)

SET(MemoryAllocatorSources
  "memory/allocators/allocator.h"
  "memory/allocators/allocator.cc"
  "memory/allocators/malloc_allocator.h"
  "memory/allocators/malloc_allocator.cc"
  "memory/allocators/eastl_allocator.h"
  "memory/allocators/eastl_allocator.cc"
)

SET(ContainerSources
  "containers/string.h"
  "containers/string_utils.h"
  "containers/string_utils.cc"
  "containers/vector.h"
  "containers/map.h"
  "containers/queue.h"
)

SET(AuxiliarySources
  "auxiliary/logger.h"
  "auxiliary/logger.cc"
  "auxiliary/pointer_math.h"
  "auxiliary/pointer_math.cc"
)

SET(IOSources
  "io/path.h"
  "io/path.cc"
  "io/file.h"
  "io/file.cc"
  "io/directory.h"
  "io/directory.cc"
  "io/resources.h"
  "io/resources.cc"
)

SET(FoundationSources
  ${DefinitionsSources}
  ${MemorySources}
  ${MemoryAllocatorSources}
  ${ContainerSources}
  ${AuxiliarySources}
  ${IOSources}
)

SOURCE_GROUP("definitions"        FILES ${DefinitionsSources})
SOURCE_GROUP("memory"             FILES ${MemorySources})
SOURCE_GROUP("memory\\allocator"  FILES ${MemoryAllocatorSources})
SOURCE_GROUP("containers"         FILES ${ContainerSources})
SOURCE_GROUP("auxiliary"          FILES ${AuxiliarySources})
SOURCE_GROUP("io"                 FILES ${IOSources})

ADD_LIBRARY(snuffbox-foundation ${FoundationSources})
TARGET_LINK_LIBRARIES(snuffbox-foundation PUBLIC eastl glm)