SET(DefinitionsSources
  "definitions/error_codes.h"
  "definitions/logging.h"
  "definitions/io.h"
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
  "containers/vector.h"
  "containers/map.h"
  "containers/queue.h"
)

SET(AuxiliarySources
  "auxiliary/logger.h"
  "auxiliary/logger.cc"
  "auxiliary/pointer_math.h"
  "auxiliary/pointer_math.cc"
  "auxiliary/string_utils.h"
  "auxiliary/string_utils.cc"
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

IF (SNUFF_WIN32)
  SET(PlatformFilter "win32")
  SET(PlatformSources
    "win32/win32_directory.h"
    "win32/win32_directory.cc"
  )
ELSEIF (SNUFF_LINUX)
  SET(PlatformFilter "linux")
  SET(PlatformSources
    "linux/linux_directory.h"
    "linux/linux_directory.cc"
  )
ELSE ()
  MESSAGE(FATAL_ERROR "Unknown compiler platform")
ENDIF (SNUFF_WIN32)

SET(EncryptionSources
  "encryption/rc4.h"
  "encryption/rc4.cc"
)

SOURCE_GROUP(${PlatformFilter}    FILES ${PlatformSources})
SOURCE_GROUP("definitions"        FILES ${DefinitionsSources})
SOURCE_GROUP("memory"             FILES ${MemorySources})
SOURCE_GROUP("memory\\allocator"  FILES ${MemoryAllocatorSources})
SOURCE_GROUP("containers"         FILES ${ContainerSources})
SOURCE_GROUP("auxiliary"          FILES ${AuxiliarySources})
SOURCE_GROUP("io"                 FILES ${IOSources})
SOURCE_GROUP("encryption"         FILES ${EncryptionSources})

SET(FoundationSources
  ${PlatformSources}
  ${DefinitionsSources}
  ${MemorySources}
  ${MemoryAllocatorSources}
  ${ContainerSources}
  ${AuxiliarySources}
  ${IOSources}
  ${EncryptionSources}
)

ADD_LIBRARY(snuffbox-foundation ${FoundationSources})
TARGET_LINK_LIBRARIES(snuffbox-foundation PUBLIC eastl glm)