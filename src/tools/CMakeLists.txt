ADD_SUBDIRECTORY("builder")
ADD_SUBDIRECTORY("editor")

IF (NOT SNUFF_NSCRIPTING)
  ADD_SUBDIRECTORY("sparse")
  
  SET_SOLUTION_FOLDER("snuffbox-hydra/tools"
    snuffbox-sparse
  )
ENDIF (NOT SNUFF_NSCRIPTING)

SET_SOLUTION_FOLDER("snuffbox-hydra/tools"
  snuffbox-editor
)