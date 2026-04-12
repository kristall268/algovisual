function(av_compile_shaders TARGET)
    cmake_parse_arguments(ARG "" "OUTPUT_DIR" "SOURCES" ${ARGN})
    if(NOT GLSLANG_VALIDATOR)
        return()
    endif()
    file(MAKE_DIRECTORY ${ARG_OUTPUT_DIR})
    set(SPV_FILES "")
    foreach(SHADER ${ARG_SOURCES})
        get_filename_component(NAME ${SHADER} NAME)
        set(OUT ${ARG_OUTPUT_DIR}/${NAME}.spv)
        add_custom_command(
            OUTPUT ${OUT}
            COMMAND ${GLSLANG_VALIDATOR} -V ${SHADER} -o ${OUT}
            DEPENDS ${SHADER}
            COMMENT "Compiling shader ${NAME}"
            VERBATIM
        )
        list(APPEND SPV_FILES ${OUT})
    endforeach()
    add_custom_target(${TARGET}_shaders DEPENDS ${SPV_FILES})
    add_dependencies(${TARGET} ${TARGET}_shaders)
endfunction()
