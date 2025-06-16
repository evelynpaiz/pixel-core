
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

