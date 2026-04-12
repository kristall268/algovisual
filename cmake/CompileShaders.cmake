function(av_compile_shaders TARGET)
    cmake_parse_arguments(ARG "" "OUTPUT_DIR" "SOURCES" ${ARGN})

    if(NOT GLSLANG_VALIDATOR)
        message(WARNING "glslangValidator not found — skipping shaders for ${TARGET}")
        return()
    endif()

    # Промежуточная папка внутри build/ — без генератор-выражений
    set(STAGE_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders_stage")
    file(MAKE_DIRECTORY ${STAGE_DIR})

    set(SPV_FILES "")
    foreach(SHADER ${ARG_SOURCES})
        if(NOT EXISTS ${SHADER})
            message(WARNING "Shader source not found, skipping: ${SHADER}")
            continue()
        endif()
        get_filename_component(NAME ${SHADER} NAME)
        set(OUT "${STAGE_DIR}/${NAME}.spv")
        add_custom_command(
            OUTPUT ${OUT}
            COMMAND ${GLSLANG_VALIDATOR} -V ${SHADER} -o ${OUT}
            DEPENDS ${SHADER}
            COMMENT "Compiling shader ${NAME}"
            VERBATIM
        )
        list(APPEND SPV_FILES ${OUT})
    endforeach()

    if(NOT SPV_FILES)
        return()
    endif()

    # Цель-агрегатор для всех .spv
    add_custom_target(${TARGET}_shaders DEPENDS ${SPV_FILES})
    add_dependencies(${TARGET} ${TARGET}_shaders)

    # Копируем рядом с exe через POST_BUILD — тут TARGET_FILE_DIR разрешается корректно
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${ARG_OUTPUT_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SPV_FILES} ${ARG_OUTPUT_DIR}
        COMMENT "Copying SPIR-V shaders to ${ARG_OUTPUT_DIR}"
        VERBATIM
    )
endfunction()
