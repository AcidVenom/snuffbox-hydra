SET(DefinitionsSources
  "definitions/error_codes.h"
  "definitions/logging.h"
  "definitions/io.h"
  "definitions/time_units.h"
  "definitions/archive.h"
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
  "memory/allocators/rapidjson_allocator.h"
  "memory/allocators/rapidjson_allocator.cc"
)

SET(ContainerSources
  "containers/string.h"
  "containers/vector.h"
  "containers/map.h"
  "containers/queue.h"
  "containers/function.h"
)

SET(AuxiliarySources
  "auxiliary/logger.h"
  "auxiliary/logger.cc"
  "auxiliary/pointer_math.h"
  "auxiliary/pointer_math.cc"
  "auxiliary/string_utils.h"
  "auxiliary/string_utils.cc"
  "auxiliary/timer.h"
  "auxiliary/timer.cc"
  "auxiliary/type_traits.h"
)

SET(IOSources
  "io/path.h"
  "io/path.cc"
  "io/file.h"
  "io/file.cc"
  "io/directory.h"
  "io/directory_listener.h"
  "io/directory_tree.h"
  "io/directory_tree.cc"
  "io/resources.h"
  "io/resources.cc"
)

SET(SerializationSources
  "serialization/save_archive.h"
  "serialization/save_archive.cc"
  "serialization/load_archive.h"
  "serialization/load_archive.cc"
  "serialization/serializable.h"
)

IF (SNUFF_WIN32)
  SET(PlatformFilter "win32")
  SET(PlatformSources
    "win32/win32_include.h"
    "win32/win32_directory.h"
    "win32/win32_directory.cc"
    "win32/win32_directory_listener.h"
    "win32/win32_directory_listener.cc"
  )
ELSEIF (SNUFF_LINUX)
  SET(PlatformFilter "linux")
  SET(PlatformSources
    "linux/linux_directory.h"
    "linux/linux_directory.cc"
    "linux/linux_directory_listener.h"
    "linux/linux_directory_listener.cc"
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
SOURCE_GROUP("serialization"      FILES ${SerializationSources})
SOURCE_GROUP("encryption"         FILES ${EncryptionSources})

SET(FoundationSources
  ${PlatformSources}
  ${DefinitionsSources}
  ${MemorySources}
  ${MemoryAllocatorSources}
  ${ContainerSources}
  ${AuxiliarySources}
  ${IOSources}
  ${SerializationSources}
  ${EncryptionSources}
)

ADD_LIBRARY(snuffbox-foundation ${FoundationSources})
TARGET_LINK_LIBRARIES(snuffbox-foundation PUBLIC eastl glm rapidjson)
