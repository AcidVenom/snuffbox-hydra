FUNCTION (SET_SOLUTION_FOLDER arg1)
  IF (MSVC)
    SET(SolutionFolder "${arg1}")
    FOREACH (Target ${ARGV})
      IF (NOT "${Target}" STREQUAL "${SolutionFolder}")
        SET_TARGET_PROPERTIES("${Target}" PROPERTIES FOLDER "${SolutionFolder}")
      ENDIF ()
    ENDFOREACH ()
  ENDIF (MSVC)
ENDFUNCTION (SET_SOLUTION_FOLDER)

FUNCTION (CREATE_SCRIPT_BINDS arg1 arg2)
  FOREACH (Relative ${arg1})
    IF(${Relative} MATCHES "\\.h")

      SET(InputFile "${CMAKE_CURRENT_SOURCE_DIR}/${Relative}")
      
      MESSAGE(STATUS "Adding 'sparse' step for ${InputFile}")
      
      STRING(REGEX REPLACE ".*\\/" "" RelativePath ${InputFile})
      STRING(REGEX REPLACE "\\.h" "" NoExtension ${RelativePath})
      
      SET(OutputFile "${arg2}/${NoExtension}.gen.cc")
      
      SET(ParseArgs "${InputFile}" -c SCRIPT_CLASS -e SCRIPT_ENUM -f SCRIPT_FUNC -m SCRIPT_NAME)
      SET(ParseTo "${NoExtension}.json")
      
      ADD_CUSTOM_COMMAND(
        OUTPUT ${OutputFile}
        COMMAND "$<TARGET_FILE:header-parser>" ${ParseArgs} > ${ParseTo} && "$<TARGET_FILE:snuffbox-sparse>" -i ${ParseTo} -o ${OutputFile} -h ${InputFile}
        DEPENDS ${InputFile}
        MAIN_DEPENDENCY ${InputFile}
        WORKING_DIRECTORY "${arg2}"
        COMMENT "------ sparse: Generating script bindings for: ${Relative}"
        VERBATIM
      )
    ENDIF ()
  ENDFOREACH ()
ENDFUNCTION(CREATE_SCRIPT_BINDS)

FUNCTION (SNUFF_BIN2H arg1 arg2)
  FOREACH (Relative ${arg1})
    SET(InputFile "${CMAKE_CURRENT_SOURCE_DIR}/${Relative}")
    
    MESSAGE(STATUS "Adding 'bin2h' step for ${Relative}")
    
    STRING(REGEX REPLACE ".*\\/" "" RelativePath ${InputFile})
    STRING(REGEX REPLACE "\\..*" "" NoExtension ${RelativePath})
    
    SET(OutputFile "${arg2}/${NoExtension}.h")
    
    SET(ConvertArgs "${InputFile}" "${Relative}" "${arg2}")
    
    ADD_CUSTOM_COMMAND(
      OUTPUT ${OutputFile}
      COMMAND "$<TARGET_FILE:snuffbox-bin2h>" ${ConvertArgs}
      DEPENDS ${InputFile}
      MAIN_DEPENDENCY ${InputFile}
      WORKING_DIRECTORY "${arg2}"
      COMMENT "------ bin2h: ${Relative} -> ${OutputFile}"
      VERBATIM
    )
  ENDFOREACH ()
ENDFUNCTION (SNUFF_BIN2H)