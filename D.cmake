
## GLEW
set(GLEW_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(GLEW_INSTALL OFF CACHE BOOL "" FORCE)
set(ONLY_LIBS ON CACHE BOOL "" FORCE)

add_subdirectory(3rdparty/glew)
add_library(glew::glew ALIAS glew)

## GLM
set(GLM_UNINSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(3rdparty/glm)

## STB
add_library(stb INTERFACE)

target_include_directories(stb
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/stb/
)

add_library(stb::stb ALIAS stb)

## ASSIMP
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)

add_subdirectory(3rdparty/assimp)

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
        ${APPLE_FWK_FOUNDATION} ${APPLE_FWK_QUARTZ_CORE} ${APPLE_FWK_METAL} Metal::Cpp
    )
endif()

source_group("include" FILES ${imgui_headers})
source_group("include/backends" FILES ${imgui_backend_headers})
source_group("src" FILES ${imgui_sources})
source_group("src/backends" FILES ${imgui_backends_sources})

add_library(imgui::imgui ALIAS imgui)