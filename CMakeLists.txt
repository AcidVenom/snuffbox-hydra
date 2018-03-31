CMAKE_MINIMUM_REQUIRED(VERSION 3.10.1)
PROJECT(snuffbox-hydra)

SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
ADD_DEFINITIONS("-DGLM_FORCE_CXX98")

IF (MSVC)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
ELSEIF (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
ENDIF ()

SET(SNUFF_BUILD_TEST ON CACHE BOOL "Should the snuffbox-test project be built?")
SET(SNUFF_BUILD_TOOLS ON CACHE BOOL "Should the tool projects be built?")

ADD_SUBDIRECTORY("cmake")
ADD_SUBDIRECTORY("deps")
ADD_SUBDIRECTORY("src")

IF (MSVC)
  SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
  SET_PROPERTY(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmake-targets")
  
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
    "snuffbox-hydra"
    snuffbox-foundation
    snuffbox-engine
    snuffbox-test
  )
ENDIF (MSVC)