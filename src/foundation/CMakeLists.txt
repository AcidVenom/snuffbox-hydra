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
  "containers/vector.h"
)

SET(AuxiliarySources
  "auxiliary/logger.h"
  "auxiliary/logger.cc"
  "auxiliary/pointer_math.h"
  "auxiliary/pointer_math.cc"
)

SET(FoundationSources
  ${MemorySources}
  ${MemoryAllocatorSources}
  ${ContainerSources}
  ${AuxiliarySources}
)

SOURCE_GROUP("memory"             FILES ${MemorySources})
SOURCE_GROUP("memory\\allocator"  FILES ${MemoryAllocatorSources})
SOURCE_GROUP("containers"         FILES ${ContainerSources})
SOURCE_GROUP("auxiliary"          FILES ${AuxiliarySources})

ADD_LIBRARY(snuffbox-foundation ${FoundationSources})
TARGET_LINK_LIBRARIES(snuffbox-foundation PUBLIC eastl glm)