cmake_minimum_required(VERSION 3.16)

# Find source files
file(
    GLOB_RECURSE sources
    LIST_DIRECTORIES false
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    
    src/Foundation/*.cpp src/Foundation/*.mm*.cpp
)

# Find platform-specific source files
if(WIN32)
    set(PLATFORM_OS_DIR "Platform/OS/Windows")
elseif(APPLE)
    set(PLATFORM_OS_DIR "Platform/OS/MacOS")
    set(PLATFORM_API_METAL_DIR "Platform/API/Metal")
else()
    set(PLATFORM_OS_DIR "Platform/OS/Generic")
endif()

set(PLATFORM_API_OPENGL_DIR "Platform/API/OpenGL")

file(
    GLOB_RECURSE platform_sources
    LIST_DIRECTORIES false
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    
    src/${PLATFORM_OS_DIR}/*.cpp
    src/${PLATFORM_OS_DIR}/*.mm

    src/${PLATFORM_API_OPENGL_DIR}/*.cpp
    src/${PLATFORM_API_OPENGL_DIR}/*.mm
)

# Find header files
file(
    GLOB_RECURSE public_headers
    LIST_DIRECTORIES false
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}

    include/pixc.h
    include/Foundation/*.h
    include/Platform/OpenGL/*.h
)

# Add platform-specific sources and headers
if (APPLE)
    file(
        GLOB_RECURSE metal_sources
        LIST_DIRECTORIES false
        RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}

        src/${PLATFORM_API_METAL_DIR}/*.cpp
        src/${PLATFORM_API_METAL_DIR}/*.mm
    )
    list(APPEND platform_sources ${metal_sources})
    
    file(
        GLOB_RECURSE metal_headers
        LIST_DIRECTORIES false
        RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        include/Platform/Metal/*.h
    )
    list(APPEND public_headers ${metal_headers})
endif()

# Find the shader files
file(
    GLOB_RECURSE shader_files
    LIST_DIRECTORIES false
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    shaders/*.glsl
    shaders/*.metal
)

# Create custom target sources (for IDE visibility only)
foreach(shader ${shader_files})
    set_source_files_properties(${shader} PROPERTIES HEADER_FILE_ONLY TRUE)
endforeach()

# Define the engine library
add_library(pixc STATIC ${sources} ${platform_sources} ${public_headers})
add_library(pixc::Engine ALIAS pixc)

# Define the include directories for this target
target_include_directories(
    pixc
    PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
)

# Define properties for the target
set_target_properties(pixc PROPERTIES
    PUBLIC_HEADER "${public_headers}"
)

# Include shader files as private
target_sources(pixc PRIVATE ${shader_files})

# Link external libraries
target_link_libraries(pixc
    spdlog::spdlog OpenGL::GL glfw::glfw glew::glew glm::glm imgui::imgui stb::stb assimp::assimp
)

# Link metal if apple device is used
if (APPLE)
    target_link_libraries(pixc
        ${APPLE_FWK_FOUNDATION} ${APPLE_FWK_QUARTZ_CORE} ${APPLE_FWK_METAL}
    )
endif()

# Add pre-processing flag
# GLM configuration: use a right-handed coordinate system and
# a [0,1] depth range to match Metal/D3D/Vulkan clip space
target_compile_definitions(pixc
    PUBLIC
        GLM_FORCE_DEPTH_ZERO_TO_ONE
        GLM_FORCE_RIGHT_HANDED
)
# STL configuration: remove depreated warnings
target_compile_definitions(pixc PRIVATE _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING)

# Define pre-compiled header
target_precompile_headers(pixc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/pixcpch.h)

# Define solution tree organization
source_group(
    TREE ${CMAKE_CURRENT_SOURCE_DIR}
    FILES ${public_headers} ${sources} ${platform_sources} ${shader_files}
)
