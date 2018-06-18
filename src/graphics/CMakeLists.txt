SET(RootSources
  "rendering.h"
  "renderer.h"
  "renderer.cc"
  "renderer_loader.h"
)

SET(DefinitionsSources
  "definitions/graphics_window.h"
  "definitions/viewport.h"
  "definitions/vertex.h"
  "definitions/texture_formats.h"
  "definitions/render_target.h"
  "definitions/frame_data.h"
  "definitions/draw_command.h"
  "definitions/shader_types.h"
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
    "ogl/ogl_context.h"
    "ogl/ogl_renderer.h"
    "ogl/ogl_renderer.cc"
    "ogl/ogl_utils.h" 
    "ogl/ogl_utils.cc"
    "ogl/ogl_loader.h"
    "ogl/ogl_loader.cc"
  )
  
  IF (SNUFF_WIN32)
    SET(ContextSources
      "ogl/win32/win32_ogl_context.h"
      "ogl/win32/win32_ogl_context.cc"
    )
    
    SOURCE_GROUP("ogl\\win32"     FILES ${ContextSources})
  ELSEIF (SNUFF_LINUX)
    SET(ContextSources
      "ogl/linux/linux_ogl_context.h"
      "ogl/linux/linux_ogl_context.cc"
    )
    
    SOURCE_GROUP("ogl\\linux"     FILES ${ContextSources})
  ENDIF ()

  SET(OGLResourcesSources
    "ogl/resources/ogl_vertex_buffer.h"
    "ogl/resources/ogl_vertex_buffer.cc"
    "ogl/resources/ogl_index_buffer.h"
    "ogl/resources/ogl_index_buffer.cc"
    "ogl/resources/ogl_mesh.h"
    "ogl/resources/ogl_mesh.cc"
    "ogl/resources/ogl_shader.h"
    "ogl/resources/ogl_shader.cc"
    "ogl/resources/ogl_uniform_buffer.h"
    "ogl/resources/ogl_uniform_buffer.cc"
    "ogl/resources/ogl_material.h"
    "ogl/resources/ogl_material.cc"
  )

  SOURCE_GROUP("ogl\\resources"   FILES ${OGLResourcesSources})
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

IF (SNUFF_OGL)
  SET(GraphicsSources
    ${GraphicsSources}
    ${ContextSources}
    ${OGLResourcesSources}
  )
ENDIF (SNUFF_OGL)

ADD_LIBRARY(snuffbox-graphics ${GraphicsSources})
TARGET_LINK_LIBRARIES(snuffbox-graphics snuffbox-foundation)

IF (SNUFF_D3D11)
  TARGET_LINK_LIBRARIES(snuffbox-graphics "d3d11.lib" "dxgi.lib")
ELSEIF (SNUFF_D3D12)
  MESSAGE(FATAL_ERROR "D3D12 is currently unsupported")
ELSEIF (SNUFF_OGL)
  TARGET_LINK_LIBRARIES(snuffbox-graphics glad)
  
  IF (SNUFF_WIN32)
    TARGET_LINK_LIBRARIES(snuffbox-graphics "opengl32.lib")
  ELSEIF (SNUFF_LINUX)
    TARGET_LINK_LIBRARIES(snuffbox-graphics "GL" "X11")
  ENDIF (SNUFF_WIN32)
ENDIF (SNUFF_D3D11)
