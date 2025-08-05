cmake_minimum_required(VERSION 3.16)

# Find source files
file(
    GLOB_RECURSE sources
    LIST_DIRECTORIES false
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    src/*.cpp src/*.h include/*.h
)

# Define the executable
add_executable(Sandbox ${sources})
add_executable(pixc::Sandbox ALIAS Sandbox)

#add_dependencies(Sandbox Resources)

# Define include directories
target_include_directories(
    Sandbox
    PRIVATE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
)

# Link external libraries
target_link_libraries(Sandbox PRIVATE pixc::Engine)

# Define the target properties
get_filename_component(renderer_dir "${CMAKE_CURRENT_SOURCE_DIR}" DIRECTORY)
set_target_properties(Sandbox PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${renderer_dir}"  # If resources copied: CMAKE_BINARY_DIR
    XCODE_GENERATE_SCHEME TRUE
    XCODE_SCHEME_ENABLE_GPU_API_VALIDATION FALSE
    XCODE_SCHEME_WORKING_DIRECTORY "${renderer_dir}"
)

# Add pre-processing flag
target_compile_definitions(Sandbox PRIVATE _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING)

# Define solution tree organization
source_group(
    TREE ${CMAKE_CURRENT_SOURCE_DIR}
    FILES ${sources}
)
