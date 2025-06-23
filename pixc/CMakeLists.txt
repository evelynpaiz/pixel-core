cmake_minimum_required(VERSION 3.16)

# Find source files
file(
    GLOB_RECURSE sources
    LIST_DIRECTORIES false
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    #   src/Foundation/*.cpp src/Foundation/*.mm

    src/Foundation/Core/*.cpp
    src/Foundation/Input/*.cpp
    src/Foundation/Layer/*.cpp

    src/Foundation/Renderer/GraphicsContext.cpp
    src/Foundation/Renderer/RendererAPI.cpp
    src/Foundation/Renderer/RendererCommand.cpp
    src/Foundation/Renderer/Renderer.cpp
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

    src/${PLATFORM_API_OPENGL_DIR}/Gui/*.cpp
    src/${PLATFORM_API_OPENGL_DIR}/OpenGLContext.cpp
    src/${PLATFORM_API_OPENGL_DIR}/OpenGLRendererAPI.cpp

#    src/${PLATFORM_API_OPENGL_DIR}/*.cpp
#    src/${PLATFORM_API_OPENGL_DIR}/*.mm
)

# Find header files
file(
    GLOB_RECURSE public_headers
    LIST_DIRECTORIES false
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
#   include/*.h

    include/pixc.h

    include/Foundation/Core/*.h
    include/Foundation/Event/*.h
    include/Foundation/Input/*.h

    include/Foundation/Layer/*.h

    include/Foundation/Renderer/GraphicsContext.h
    include/Foundation/Renderer/RendererAPI.h
    include/Foundation/Renderer/RendererCommand.h
    include/Foundation/Renderer/Renderer.h

    include/Platform/OpenGL/Gui/*.h
    include/Platform/OpenGL/OpenGLContext.h
    include/Platform/OpenGL/OpenGLRendererAPI.h
)

# Add platform-specific sources and headers
if (APPLE)
    list(APPEND platform_sources
        src/${PLATFORM_API_METAL_DIR}/Gui/MetalGuiBackend.mm
        src/${PLATFORM_API_METAL_DIR}/MetalContext.mm
        src/${PLATFORM_API_METAL_DIR}/MetalRendererAPI.mm
        
    )

    list(APPEND public_headers
        include/Platform/Metal/Gui/MetalGuiBackend.h
        include/Platform/Metal/MetalContext.h
        include/Platform/Metal/MetalRendererAPI.h
    )
endif()

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

# Link external libraries
target_link_libraries(pixc 
    spdlog::spdlog OpenGL::GL glfw::glfw glew::glew glm::glm imgui::imgui
#   stb::stb assimp::assimp
)

# Link metal if apple device is used
if (APPLE)
    target_link_libraries(pixc
        ${APPLE_FWK_FOUNDATION} ${APPLE_FWK_QUARTZ_CORE} ${APPLE_FWK_METAL}
    )
endif()

# Define pre-compiled header
target_precompile_headers(pixc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/enginepch.h)

# Add pre-processing flag
target_compile_definitions(pixc PRIVATE _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING)

# Define solution tree organization
source_group(
    TREE ${CMAKE_CURRENT_SOURCE_DIR}
    FILES ${public_headers} ${sources} ${platform_sources}
)
