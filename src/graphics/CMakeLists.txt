SET(RootSources
  "renderer.h"
)

IF (SNUFF_D3D11)
  SET(PlatformPrefix "d3d11")
  SET(PlatformSources
    "d3d11/d3d11_renderer.h"
    "d3d11/d3d11_renderer.cc"
  )
ELSEIF (SNUFF_D3D12)
  SET(PlatformPrefix "d3d12")
  SET(PlatformSources
    "d3d12/d3d12_renderer.h"
    "d3d12/d3d12_renderer.cc"
  )
ELSEIF (SNUFF_OGL)
  SET(PlatformPrefix "ogl")
  SET(PlatformSources
    "ogl/ogl_renderer.h"
    "ogl/ogl_renderer.cc"
  )
ELSE ()
  MESSAGE(FATAL_ERROR "Unsupported renderer implementation")
ENDIF ()

SOURCE_GROUP("\\"                 FILES ${RootSources})
SOURCE_GROUP("${PlatformPrefix}"  FILES ${PlatformSources})

SET(GraphicsSources
  ${RootSources}
  ${PlatformSources}
)

ADD_LIBRARY(snuffbox-graphics ${GraphicsSources})
TARGET_LINK_LIBRARIES(snuffbox-graphics snuffbox-foundation)

IF (SNUFF_D3D11)
  TARGET_LINK_LIBRARIES(snuffbox-graphics "d3d11.lib")
ELSEIF (SNUFF_D3D12)
  MESSAGE(FATAL_ERROR "D3D12 is currently unsupported")
ELSEIF (SNUFF_OGL)
  TARGET_LINK_LIBRARIES(snuffbox-graphics glad)
ENDIF (SNUFF_D3D11)