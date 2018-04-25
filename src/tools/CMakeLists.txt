ADD_SUBDIRECTORY("bin2h")
ADD_SUBDIRECTORY("builder")

SET_SOLUTION_FOLDER("snuffbox-hydra/tools"
  snuffbox-bin2h
  snuffbox-builder
)

IF (SNUFF_BUILD_EDITOR)
  ADD_SUBDIRECTORY("editor")

  SET_SOLUTION_FOLDER("snuffbox-hydra/tools"
    snuffbox-editor
  )
ENDIF (SNUFF_BUILD_EDITOR)

IF (NOT SNUFF_NSCRIPTING)
  ADD_SUBDIRECTORY("sparse")
  
  SET_SOLUTION_FOLDER("snuffbox-hydra/tools"
    snuffbox-sparse
  )
ENDIF (NOT SNUFF_NSCRIPTING)
