# Disable warnings for all dependencies

SET(BUILD_TESTING OFF CACHE BOOL "" FORCE)

IF (SNUFF_WIN32)
  IF (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    STRING(REGEX REPLACE "/W[0-4]" "/W0" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  ELSE ()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W0")
  ENDIF ()
ELSEIF (SNUFF_LINUX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
ELSE ()
  MESSAGE(FATAL_ERROR "Unknown compiler platform")
ENDIF (SNUFF_WIN32)

SET(EASTL_BUILD_TESTS OFF CACHE BOOL "" FORCE)
ADD_SUBDIRECTORY("eastl")
ADD_SUBDIRECTORY("eastl/test/packages/EAAssert")
ADD_SUBDIRECTORY("eastl/test/packages/EAStdC")
ADD_SUBDIRECTORY("eastl/test/packages/EAMain")
ADD_SUBDIRECTORY("eastl/test/packages/EATest")
ADD_SUBDIRECTORY("eastl/test/packages/EAThread")

ADD_LIBRARY(eastl INTERFACE)
TARGET_LINK_LIBRARIES(eastl INTERFACE EASTL EAAssert EAStdC EAMain EATest EAThread)
TARGET_INCLUDE_DIRECTORIES(eastl INTERFACE "eastl/include")

SET(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
SET(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
SET(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
ADD_SUBDIRECTORY("glfw")

ADD_LIBRARY(glm INTERFACE)
TARGET_INCLUDE_DIRECTORIES(glm INTERFACE "glm")

IF (SNUFF_OGL)
  ADD_SUBDIRECTORY("glad")
  SET_SOLUTION_FOLDER(
    "deps/glad"
    glad
  )
ENDIF (SNUFF_OGL)

IF (SNUFF_DUKTAPE)
  ADD_SUBDIRECTORY("duktape-2.2.0")
  SET_SOLUTION_FOLDER(
    "deps/duktape"
    duktape
  )
ENDIF (SNUFF_DUKTAPE)

IF (NOT SNUFF_NSCRIPTING)
  ADD_SUBDIRECTORY("header-parser")
  
  SET_SOLUTION_FOLDER(
    "deps/header-parser"
    header-parser
  )
ENDIF (NOT SNUFF_NSCRIPTING)

ADD_LIBRARY(rapidjson INTERFACE)
TARGET_INCLUDE_DIRECTORIES(rapidjson INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/include")

SET(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "" FORCE)
ADD_SUBDIRECTORY("glslang")
ADD_LIBRARY(glslang-lib INTERFACE)
TARGET_LINK_LIBRARIES(glslang-lib INTERFACE
  glslang
  HLSL
  OSDependent
  OGLCompiler
  SPIRV
)

ADD_SUBDIRECTORY("spirv-cross")
ADD_LIBRARY(spirv-cross-lib INTERFACE)
TARGET_LINK_LIBRARIES(spirv-cross-lib INTERFACE 
  spirv-cross-core 
  spirv-cross-cpp 
  spirv-cross-util 
  spirv-cross-hlsl 
  spirv-cross-glsl
)

ADD_LIBRARY(tinygltf INTERFACE)
TARGET_INCLUDE_DIRECTORIES(tinygltf INTERFACE "tinygltf")

SET_SOLUTION_FOLDER(
  "deps/eastl"
  EABase_ide
  EASTL
  EAAssert_ide
  EAMain_ide
  EAStdC
  EATest
  EAThread_ide
)

SET_SOLUTION_FOLDER(
  "deps/glfw3"
  glfw
  uninstall
)

SET_SOLUTION_FOLDER(
  "deps/glslang"
  glslang
  OGLCompiler
  OSDependent
  SPIRV
  SPVRemapper
  HLSL
)

SET_SOLUTION_FOLDER(
  "deps/spirv-cross"
  spirv-cross
  spirv-cross-core
  spirv-cross-cpp
  spirv-cross-glsl
  spirv-cross-hlsl
  spirv-cross-msl
  spirv-cross-util
)
