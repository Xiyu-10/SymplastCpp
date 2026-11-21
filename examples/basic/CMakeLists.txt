add_executable(symplast_example_basic
    WIN32
    main.cc
)

target_link_libraries(symplast_example_basic
    PRIVATE
        symplast::symplast
)

add_custom_command(
    TARGET symplast_example_basic POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/hello.html"
        "$<TARGET_FILE_DIR:symplast_example_basic>/hello.html"
)