function(av_copy_assets TARGET)
    cmake_parse_arguments(ARG "" "TO" "FROM" ${ARGN})
    foreach(DIR ${ARG_FROM})
        get_filename_component(NAME ${DIR} NAME)
        add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${DIR} ${ARG_TO}/${NAME}
            COMMENT "Copying assets: ${NAME}"
        )
    endforeach()
endfunction()
