IF (SNUFF_BUILD_TOOLS)
  ADD_SUBDIRECTORY("builder")
  ADD_SUBDIRECTORY("editor")

  SET_SOLUTION_FOLDER("snuffbox-hydra/tools"
    snuffbox-editor
  )
ENDIF (SNUFF_BUILD_TOOLS)

IF (NOT SNUFF_NSCRIPTING)
  ADD_SUBDIRECTORY("sparse")
  
  SET_SOLUTION_FOLDER("snuffbox-hydra/tools"
    snuffbox-sparse
  )
ENDIF (NOT SNUFF_NSCRIPTING)
