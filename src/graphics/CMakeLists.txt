SET(RootSources
  "rendering.h"
  "renderer.h"
  "renderer.cc"
)

SET(DefinitionsSources
  "definitions/graphics_window.h"
  "definitions/viewport.h"
  "definitions/vertex.h"
  "definitions/texture_formats.h"
  "definitions/render_target.h"
)

IF (SNUFF_WIN32)
  SET(DefinitionsSources
    ${DefinitionsSources}
    "definitions/com_ptr.h"
  )
ENDIF (SNUFF_WIN32)

IF (SNUFF_D3D11)
  SET(PlatformPrefix "d3d11")
  SET(PlatformSources
    "d3d11/d3d11_renderer.h"
    "d3d11/d3d11_renderer.cc"
    "d3d11/d3d11_utils.h"
    "d3d11/d3d11_utils.cc"
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
SOURCE_GROUP("definitions"        FILES ${DefinitionsSources})
SOURCE_GROUP("${PlatformPrefix}"  FILES ${PlatformSources})

SET(GraphicsSources
  ${RootSources}
  ${DefinitionsSources}
  ${PlatformSources}
)

ADD_LIBRARY(snuffbox-graphics ${GraphicsSources})
TARGET_LINK_LIBRARIES(snuffbox-graphics snuffbox-foundation)

IF (SNUFF_D3D11)
  TARGET_LINK_LIBRARIES(snuffbox-graphics "d3d11.lib" "dxgi.lib")
ELSEIF (SNUFF_D3D12)
  MESSAGE(FATAL_ERROR "D3D12 is currently unsupported")
ELSEIF (SNUFF_OGL)
  TARGET_LINK_LIBRARIES(snuffbox-graphics glad)
ENDIF (SNUFF_D3D11)