cmake_minimum_required(VERSION 3.16)

# Define the resources
file(GLOB_RECURSE common RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} common/*)

file(GLOB_RECURSE textures RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} textures/*)
file(GLOB_RECURSE models RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} models/*)

file(GLOB_RECURSE shaders_glsl RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} shaders/*.glsl)
file(GLOB_RECURSE shaders_metal RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} shaders/*.h shaders/*.metal)
set(shaders ${shaders_glsl} ${shaders_metal})

set(resources_files ${common} ${textures} ${models} ${shaders})

# Define the resources library
add_library(resources INTERFACE ${resources_files})

# Define solution tree organization
source_group(
    TREE ${CMAKE_CURRENT_SOURCE_DIR}
    FILES ${resources_files}
)

# NOTE: Use the following to copy the resources to the binary directory
# set(outputs)
# foreach (resource ${resources})
#     string(REPLACE "/" "_" name ${resource})
#     set(input ${CMAKE_CURRENT_SOURCE_DIR}/${resource})
#     set(output ${CMAKE_BINARY_DIR}/Resources/${resource})
#     add_custom_command(
#         OUTPUT ${output}
#         DEPENDS ${input}
#         COMMAND ${CMAKE_COMMAND} -E copy ${input} ${output}
#     )
#     list(APPEND outputs ${output})
# endforeach()

# source_group(""
#     FILES ${outputs}
#     REGULAR_EXPRESSION "^resources/.*"
# )
