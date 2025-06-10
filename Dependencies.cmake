# Third party libraries

## SPDLOG
set(SPDLOG_BUILD_ONLY_HEADERS ON CACHE BOOL "" FORCE)
add_subdirectory(3rdparty/spdlog)

## GLFW
set(GLFW_MAPPING OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(3rdparty/glfw)
add_library(glfw::glfw ALIAS glfw)

set_target_properties(glfw PROPERTIES FOLDER "")

## Metal
if(APPLE)
    add_library(metalcpp INTERFACE)
    target_include_directories(metalcpp
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/metal-cpp/
    )
    add_library(Metal::Cpp ALIAS metalcpp)
endif()
