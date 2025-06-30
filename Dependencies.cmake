# Third party libraries

## SPDLOG
set(SPDLOG_BUILD_ONLY_HEADERS ON CACHE BOOL "" FORCE)
add_subdirectory(3rdparty/spdlog)

add_library(spdlog::spdlog ALIAS spdlog_header_only)

## GLFW
set(GLFW_MAPPING OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(3rdparty/glfw)
add_library(glfw::glfw ALIAS glfw)

set_target_properties(glfw PROPERTIES FOLDER "")

## GLEW
set(GLEW_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(GLEW_INSTALL OFF CACHE BOOL "" FORCE)
set(ONLY_LIBS ON CACHE BOOL "" FORCE)

add_subdirectory(3rdparty/glew)
add_library(glew::glew ALIAS glew)


## GLM
set(GLM_BUILD_LIBRARY OFF CACHE BOOL "" FORCE)

add_subdirectory(3rdparty/glm)

## STB
add_library(stb INTERFACE)

target_include_directories(stb
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/stb/
)

add_library(stb::stb ALIAS stb)

## IMGUI
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui/)
add_library(imgui STATIC)

file(GLOB imgui_headers ${IMGUI_DIR}/*.h)

file(GLOB imgui_backend_headers 
    ${IMGUI_DIR}/backends/imgui_impl_glfw.h
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.h
    ${IMGUI_DIR}/backends/imgui_impl_metal.h
)

file(
    GLOB imgui_sources
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui.cpp
)

file(
    GLOB imgui_backends_sources
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_metal.mm
)

target_sources(imgui 
    PRIVATE ${imgui_sources}
    PRIVATE ${imgui_backends_sources}
    PRIVATE ${imgui_headers}
    PRIVATE ${imgui_backend_headers}
)

target_include_directories(imgui
    PUBLIC ${IMGUI_DIR}
    PUBLIC ${IMGUI_DIR}/backends
)

target_link_libraries(imgui OpenGL::GL glfw::glfw)
if (APPLE)
    target_link_libraries(imgui
        ${APPLE_FWK_FOUNDATION} ${APPLE_FWK_QUARTZ_CORE} ${APPLE_FWK_METAL}
    )
endif()

source_group("include" FILES ${imgui_headers})
source_group("include/backends" FILES ${imgui_backend_headers})
source_group("src" FILES ${imgui_sources})
source_group("src/backends" FILES ${imgui_backends_sources})

add_library(imgui::imgui ALIAS imgui)