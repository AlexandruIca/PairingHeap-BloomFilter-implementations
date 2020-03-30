set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(${CMAKE_PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
                                     ${CMAKE_CURRENT_SOURCE_DIR}/test.txt)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE project_options
                                                    project_warnings)
